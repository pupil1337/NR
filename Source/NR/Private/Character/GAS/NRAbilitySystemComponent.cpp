// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/NRAbilitySystemComponent.h"

#include "AbilitySystemLog.h"
#include "Character/GAS/NRGameplayAbility.h"
#include "Net/UnrealNetwork.h"
#include "Net/Serialization/FastArraySerializer.h"

static TAutoConsoleVariable<float> CVarReplayMontageErrorThreshold(
	TEXT("NR.replay.MontageErrorThreshold"),
	0.5f,
	TEXT("Tolerance level for when montage playback position correction occurs in replays"));

UNRAbilitySystemComponent::UNRAbilitySystemComponent()
{
}

void UNRAbilitySystemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UNRAbilitySystemComponent, RepAnimMontageInfoForMeshes, COND_None)
}

void UNRAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	LocalAnimMontageInfoForMeshes = TArray<FGameplayAbilityLocalAnimMontageForMesh>();
	RepAnimMontageInfoForMeshes = TArray<FGameplayAbilityRepAnimMontageForMesh>();

	if (bPendingMontageRepForMesh)
	{
		OnRep_ReplicatedAnimMontageForMeshes();
	}
}

bool UNRAbilitySystemComponent::GetShouldTick() const
{
	for (const FGameplayAbilityRepAnimMontageForMesh& RepAnimMontageForMesh: RepAnimMontageInfoForMeshes)
	{
		if (IsOwnerActorAuthoritative() && RepAnimMontageForMesh.RepAnimMontageInfo.IsStopped == false)
		{
			return true;
		}
	}
	
	return Super::GetShouldTick();
}

void UNRAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (IsOwnerActorAuthoritative())
	{
		for (const FGameplayAbilityLocalAnimMontageForMesh& LocalAnimMontageForMesh: LocalAnimMontageInfoForMeshes)
		{
			AnimMontage_UpdateReplicatedDataForMesh(LocalAnimMontageForMesh.Mesh);
		}
	}
	
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UNRAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);

	// If AnimatingAbility ended, clear the pointer
	ClearAnimatingAbilityForAllMeshes(Ability);
}

float UNRAbilitySystemComponent::PlayMontageForMesh(UGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, USkeletalMeshComponent* InMesh, UAnimMontage* NewAnimMontage, float InPlayRate, FName StartSectionName, float StartTimeSeconds)
{
	float Duration = -1.0f;

	if (NewAnimMontage)
	{
		if (UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo->AvatarActor ? InMesh->GetAnimInstance() : nullptr)
		{
			Duration = AnimInstance->Montage_Play(NewAnimMontage, InPlayRate, EMontagePlayReturnType::MontageLength, StartTimeSeconds);
			if (Duration > 0.0f)
			{
				FGameplayAbilityLocalAnimMontageForMesh& LocalAnimMontageForMesh = GetLocalAnimMontageInfoForMesh(InMesh);
				if (LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimatingAbility.IsValid() && LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimatingAbility.Get() != AnimatingAbility)
				{
					// The ability that was previously animating will have already gotten the 'interrupted' callback.
					// It may be a good idea to make this a global policy and 'cancel' the ability.
					// 
					// For now, we expect it to end itself when this happens.
				}

				if (NewAnimMontage->HasRootMotion() && AnimInstance->GetOwningActor())
				{
					UE_LOG(LogRootMotion, Log, TEXT("UAbilitySystemComponent::PlayMontage %s, Role: %s")
						, *GetNameSafe(NewAnimMontage)
						, *UEnum::GetValueAsString(TEXT("Engine.ENetRole"), AnimInstance->GetOwningActor()->GetLocalRole())
					);
				}

				LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage = NewAnimMontage;
				LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimatingAbility = AnimatingAbility;
				LocalAnimMontageForMesh.LocalAnimMontageInfo.PlayInstanceId = (LocalAnimMontageForMesh.LocalAnimMontageInfo.PlayInstanceId < UINT8_MAX ? LocalAnimMontageForMesh.LocalAnimMontageInfo.PlayInstanceId + 1 : 0);

				if (UNRGameplayAbility* NRGameplayAbility = Cast<UNRGameplayAbility>(AnimatingAbility))
				{
					NRGameplayAbility->SetCurrentAbilityMeshMontage(InMesh, NewAnimMontage);
				}
			
				// Start at a given Section.
				if (StartSectionName != NAME_None)
				{
					AnimInstance->Montage_JumpToSection(StartSectionName, NewAnimMontage);
				}
				
				// Replicate for non-owners and for replay recordings
				// The data we set from GetRepAnimMontageInfo_Mutable() is used both by the server to replicate to clients and by clients to record replays.
				// We need to set this data for recording clients because there exists network configurations where an abilities montage data will not replicate to some clients (for example: if the client is an autonomous proxy.)
				if (ShouldRecordMontageReplication())
				{
					FGameplayAbilityRepAnimMontageForMesh& RepAnimMontageInfoForMesh = GetRepAnimMontageInfoForMesh(InMesh);

					// Those are static parameters, they are only set when the montage is played. They are not changed after that.
					RepAnimMontageInfoForMesh.RepAnimMontageInfo.AnimMontage = NewAnimMontage;
					RepAnimMontageInfoForMesh.RepAnimMontageInfo.PlayInstanceId = (RepAnimMontageInfoForMesh.RepAnimMontageInfo.PlayInstanceId < UINT8_MAX ? RepAnimMontageInfoForMesh.RepAnimMontageInfo.PlayInstanceId + 1 : 0);

					RepAnimMontageInfoForMesh.RepAnimMontageInfo.SectionIdToPlay = 0;
					if (RepAnimMontageInfoForMesh.RepAnimMontageInfo.AnimMontage && StartSectionName != NAME_None)
					{
						// we add one so INDEX_NONE can be used in the on rep
						RepAnimMontageInfoForMesh.RepAnimMontageInfo.SectionIdToPlay = RepAnimMontageInfoForMesh.RepAnimMontageInfo.AnimMontage->GetSectionIndex(StartSectionName) + 1;
					}

					// Update parameters that change during Montage life time.
					AnimMontage_UpdateReplicatedDataForMesh(InMesh);
				}

				// Replicate to non-owners
				if (IsOwnerActorAuthoritative())
				{
					// Force net update on our avatar actor.
					if (AbilityActorInfo->AvatarActor != nullptr)
					{
						AbilityActorInfo->AvatarActor->ForceNetUpdate();
					}
				}
				else
				{
					// If this prediction key is rejected, we need to end the preview
					FPredictionKey PredictionKey = GetPredictionKeyForNewAction();
					if (PredictionKey.IsValidKey())
					{
						PredictionKey.NewRejectedDelegate().BindUObject(this, &UNRAbilitySystemComponent::OnPredictiveMontageRejectedForMesh, InMesh, NewAnimMontage);
					}
				}
			}
		}	
	}

	return Duration;
}

void UNRAbilitySystemComponent::OnRep_ReplicatedAnimMontageForMeshes()
{
	for (FGameplayAbilityRepAnimMontageForMesh& RepAnimMontageForMesh: RepAnimMontageInfoForMeshes)
	{
		FGameplayAbilityLocalAnimMontageForMesh& LocalAnimMontageForMesh = GetLocalAnimMontageInfoForMesh(RepAnimMontageForMesh.Mesh);
		UWorld* World = GetWorld();

		if (RepAnimMontageForMesh.RepAnimMontageInfo.bSkipPlayRate)
		{
			RepAnimMontageForMesh.RepAnimMontageInfo.PlayRate = 1.0f;
		}

		const bool bIsPlayingReplay = World && World->IsPlayingReplay();

		const float MONTAGE_REP_POS_ERR_THRESH = bIsPlayingReplay ? CVarReplayMontageErrorThreshold.GetValueOnGameThread() : 0.1f;

		UAnimInstance* AnimInstance = IsValid(RepAnimMontageForMesh.Mesh) && AbilityActorInfo.IsValid() && RepAnimMontageForMesh.Mesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? RepAnimMontageForMesh.Mesh->GetAnimInstance() : nullptr;
		if (AnimInstance == nullptr || !IsReadyForReplicatedMontage())
		{
			// We can't handle this yet
			bPendingMontageRepForMesh = true;
			return;
		}
		bPendingMontageRepForMesh = false;

		if (!AbilityActorInfo->IsLocallyControlled())
		{
			static const auto CVar = IConsoleManager::Get().FindTConsoleVariableDataInt(TEXT("net.Montage.Debug"));
			bool DebugMontage = (CVar && CVar->GetValueOnGameThread() == 1);
			if (DebugMontage)
			{
				ABILITY_LOG( Warning, TEXT("\n\nOnRep_ReplicatedAnimMontageForMeshes, %s"), *GetNameSafe(this));
				ABILITY_LOG( Warning, TEXT("\tAnimMontage: %s\n\tPlayRate: %f\n\tPosition: %f\n\tBlendTime: %f\n\tNextSectionID: %d\n\tIsStopped: %d\n\tPlayInstanceId: %d"),
								*GetNameSafe(RepAnimMontageForMesh.RepAnimMontageInfo.AnimMontage),
								RepAnimMontageForMesh.RepAnimMontageInfo.PlayRate,
								RepAnimMontageForMesh.RepAnimMontageInfo.Position,
								RepAnimMontageForMesh.RepAnimMontageInfo.BlendTime,
								RepAnimMontageForMesh.RepAnimMontageInfo.NextSectionID,
								RepAnimMontageForMesh.RepAnimMontageInfo.IsStopped,
								RepAnimMontageForMesh.RepAnimMontageInfo.PlayInstanceId);
				ABILITY_LOG( Warning, TEXT("\tLocalAnimMontageInfo.AnimMontage: %s\n\tPosition: %f"),
					*GetNameSafe(LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage), AnimInstance->Montage_GetPosition(LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage));
			}
		}

		if (RepAnimMontageForMesh.RepAnimMontageInfo.AnimMontage)
		{
			// New Montage to play
			if ((LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage != RepAnimMontageForMesh.RepAnimMontageInfo.AnimMontage) ||
				(LocalAnimMontageForMesh.LocalAnimMontageInfo.PlayInstanceId != RepAnimMontageForMesh.RepAnimMontageInfo.PlayInstanceId))
			{
				LocalAnimMontageForMesh.LocalAnimMontageInfo.PlayInstanceId = RepAnimMontageForMesh.RepAnimMontageInfo.PlayInstanceId;
				PlayMontageSimulatedForMesh(RepAnimMontageForMesh.Mesh, RepAnimMontageForMesh.RepAnimMontageInfo.AnimMontage, RepAnimMontageForMesh.RepAnimMontageInfo.PlayRate);
			}

			if (LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage == nullptr)
			{
				ABILITY_LOG(Warning, TEXT("OnRep_ReplicatedAnimMontageForMeshes: PlayMontageSimulated failed. Name: %s, AnimMontage: %s"), *GetNameSafe(this), *GetNameSafe(LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage));
				return;
			}

			// Play Rate has changed
			if (AnimInstance->Montage_GetPlayRate(LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage) != RepAnimMontageForMesh.RepAnimMontageInfo.PlayRate)
			{
				AnimInstance->Montage_SetPlayRate(LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage, RepAnimMontageForMesh.RepAnimMontageInfo.PlayRate);			
			}

			const int32 SectionIdToPlay = (static_cast<int32>(RepAnimMontageForMesh.RepAnimMontageInfo.SectionIdToPlay) - 1);
			if (SectionIdToPlay != INDEX_NONE)
			{
				FName SectionNameToJumpTo = LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage->GetSectionName(SectionIdToPlay);
				if (SectionNameToJumpTo != NAME_None)
				{
					AnimInstance->Montage_JumpToSection(SectionNameToJumpTo, LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage);
				}
			}

			// Compressed Flags
			const bool bIsStopped = AnimInstance->Montage_GetIsStopped(LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage);
			const bool bReplicatedIsStopped = static_cast<bool>(RepAnimMontageForMesh.RepAnimMontageInfo.IsStopped);

			// Process stopping first, so we don't change sections and cause blending to pop.
			if (bReplicatedIsStopped)
			{
				if (!bIsStopped)
				{
					CurrentMontageStopForMesh(LocalAnimMontageForMesh.Mesh, RepAnimMontageForMesh.RepAnimMontageInfo.BlendTime);
				}
			}
			else if (!RepAnimMontageForMesh.RepAnimMontageInfo.SkipPositionCorrection)
			{
				const int32 RepSectionID = LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage->GetSectionIndexFromPosition(RepAnimMontageForMesh.RepAnimMontageInfo.Position);
				const int32 RepNextSectionID = static_cast<int32>(RepAnimMontageForMesh.RepAnimMontageInfo.NextSectionID) - 1;

				// And NextSectionID for the replicated SectionID.
				if (RepSectionID != INDEX_NONE)
				{
					const int32 NextSectionID = AnimInstance->Montage_GetNextSectionID(LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage, RepSectionID);

					// If NextSectionID is different than the replicated one, then set it.
					if (NextSectionID != RepNextSectionID)
					{
						AnimInstance->Montage_SetNextSection(LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage->GetSectionName(RepSectionID), LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage->GetSectionName(RepNextSectionID), LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage);
					}

					// Make sure we haven't received that update too late and the client hasn't already jumped to another section. 
					const int32 CurrentSectionID = LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage->GetSectionIndexFromPosition(AnimInstance->Montage_GetPosition(LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage));
					if ((CurrentSectionID != RepSectionID) && (CurrentSectionID != RepNextSectionID))
					{
						// Client is in a wrong section, teleport it into the beginning of the right section
						const float SectionStartTime = LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage->GetAnimCompositeSection(RepSectionID).GetTime();
						AnimInstance->Montage_SetPosition(LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage, SectionStartTime);
					}
				}

				// Update Position. If error is too great, jump to replicated position.
				const float CurrentPosition = AnimInstance->Montage_GetPosition(LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage);
				const int32 CurrentSectionID = LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage->GetSectionIndexFromPosition(CurrentPosition);
				const float DeltaPosition = RepAnimMontageForMesh.RepAnimMontageInfo.Position - CurrentPosition;

				// Only check threshold if we are located in the same section. Different sections require a bit more work as we could be jumping around the timeline.
				// And therefore DeltaPosition is not as trivial to determine.
				if ((CurrentSectionID == RepSectionID) && (FMath::Abs(DeltaPosition) > MONTAGE_REP_POS_ERR_THRESH) && (RepAnimMontageForMesh.RepAnimMontageInfo.IsStopped == 0))
				{
					// fast forward to server position and trigger notifies
					if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(RepAnimMontageForMesh.RepAnimMontageInfo.AnimMontage))
					{
						// Skip triggering notifies if we're going backwards in time, we've already triggered them.
						const float DeltaTime = !FMath::IsNearlyZero(RepAnimMontageForMesh.RepAnimMontageInfo.PlayRate) ? (DeltaPosition / RepAnimMontageForMesh.RepAnimMontageInfo.PlayRate) : 0.f;
						if (DeltaTime >= 0.f)
						{
							MontageInstance->UpdateWeight(DeltaTime);
							MontageInstance->HandleEvents(CurrentPosition, RepAnimMontageForMesh.RepAnimMontageInfo.Position, nullptr);
							AnimInstance->TriggerAnimNotifies(DeltaTime);
						}
					}
					AnimInstance->Montage_SetPosition(LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage, RepAnimMontageForMesh.RepAnimMontageInfo.Position);
				}
			}
		}
	}
}

float UNRAbilitySystemComponent::PlayMontageSimulatedForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* NewAnimMontage, float InPlayRate, FName StartSectionName)
{
	float Duration = -1.0f;
	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	if (AnimInstance && NewAnimMontage)
	{
		Duration = AnimInstance->Montage_Play(NewAnimMontage, InPlayRate);
		if (Duration > 0.0f)
		{
			GetLocalAnimMontageInfoForMesh(InMesh).LocalAnimMontageInfo.AnimMontage = NewAnimMontage;
		}
	}

	return Duration;
}

void UNRAbilitySystemComponent::CurrentMontageStopForMesh(USkeletalMeshComponent* InMesh, float OverrideBlendOutTime)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	const UAnimMontage* MontageToStop = GetLocalAnimMontageInfoForMesh(InMesh).LocalAnimMontageInfo.AnimMontage;

	if (AnimInstance && MontageToStop && !AnimInstance->Montage_GetIsStopped(MontageToStop))
	{
		const float BlendOutTime = (OverrideBlendOutTime >= 0.0f ? OverrideBlendOutTime : MontageToStop->BlendOut.GetBlendTime());

		AnimInstance->Montage_Stop(BlendOutTime, MontageToStop);

		if (IsOwnerActorAuthoritative())
		{
			AnimMontage_UpdateReplicatedDataForMesh(InMesh);
		}
	}
}

void UNRAbilitySystemComponent::StopMontageIfCurrentForMesh(USkeletalMeshComponent* InMesh, const UAnimMontage& Montage, float OverrideBlendOutTime)
{
	if (&Montage == GetLocalAnimMontageInfoForMesh(InMesh).LocalAnimMontageInfo.AnimMontage)
	{
		CurrentMontageStopForMesh(InMesh, OverrideBlendOutTime);
	}
}

void UNRAbilitySystemComponent::ClearAnimatingAbilityForAllMeshes(UGameplayAbility* Ability)
{
	if (UNRGameplayAbility* NRGameplayAbility = Cast<UNRGameplayAbility>(Ability))
	{
		for (FGameplayAbilityLocalAnimMontageForMesh& LocalAnimMontageForMesh: LocalAnimMontageInfoForMeshes)
		{
			if (LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimatingAbility.Get() == Ability)
			{
				NRGameplayAbility->SetCurrentAbilityMeshMontage(LocalAnimMontageForMesh.Mesh, nullptr);
				LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimatingAbility = nullptr;
			}
		}	
	}
}

void UNRAbilitySystemComponent::CurrentMontageJumpToSectionNameForMesh(USkeletalMeshComponent* InMesh, FName SectionName)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	UAnimMontage* AnimMontageToSet = GetLocalAnimMontageInfoForMesh(InMesh).LocalAnimMontageInfo.AnimMontage;
	if (SectionName != NAME_None && AnimInstance && AnimMontageToSet)
	{
		AnimInstance->Montage_JumpToSection(SectionName, AnimMontageToSet);

		// This data is needed for replication on the server and recording replays on clients.
		// We need to set GetRepAnimMontageInfo_Mutable on replay recording clients because this data is NOT replicated to all clients (for example, it is NOT replicated to autonomous proxy clients.)
		if (ShouldRecordMontageReplication())
		{
			FGameplayAbilityRepAnimMontageForMesh& RepAnimMontageForMesh = GetRepAnimMontageInfoForMesh(InMesh);

			RepAnimMontageForMesh.RepAnimMontageInfo.SectionIdToPlay = 0;
			if (RepAnimMontageForMesh.RepAnimMontageInfo.AnimMontage)
			{
				// we add one so INDEX_NONE can be used in the on rep
				RepAnimMontageForMesh.RepAnimMontageInfo.SectionIdToPlay = RepAnimMontageForMesh.RepAnimMontageInfo.AnimMontage->GetSectionIndex(SectionName) + 1;
			}

			AnimMontage_UpdateReplicatedDataForMesh(InMesh);
		}

		// If we are NOT the authority, then let the server handling jumping the montage.
		if (!IsOwnerActorAuthoritative())
		{
			Server_CurrentMontageJumpToSectionNameForMesh(InMesh, AnimMontageToSet, SectionName);
		}
	}
}

void UNRAbilitySystemComponent::CurrentMontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, FName FromSectionName, FName ToSectionName)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	UAnimMontage* AnimMontageToSet = GetLocalAnimMontageInfoForMesh(InMesh).LocalAnimMontageInfo.AnimMontage;
	if (AnimMontageToSet && AnimInstance)
	{
		// Set Next Section Name.
		AnimInstance->Montage_SetNextSection(FromSectionName, ToSectionName, AnimMontageToSet);

		// Update replicated version for Simulated Proxies if we are on the server.
		if (IsOwnerActorAuthoritative())
		{
			AnimMontage_UpdateReplicatedDataForMesh(InMesh);
		}
		else
		{
			float CurrentPosition = AnimInstance->Montage_GetPosition(AnimMontageToSet);
			Server_CurrentMontageSetNextSectionNameForMesh(InMesh, AnimMontageToSet, CurrentPosition, FromSectionName, ToSectionName);
		}
	}
}

void UNRAbilitySystemComponent::CurrentMontageSetPlayRateForMesh(USkeletalMeshComponent* InMesh, float InPlayRate)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	UAnimMontage* AnimMontageToSet = GetLocalAnimMontageInfoForMesh(InMesh).LocalAnimMontageInfo.AnimMontage;
	if (AnimMontageToSet && AnimInstance)
	{
		// Set Play Rate
		AnimInstance->Montage_SetPlayRate(AnimMontageToSet, InPlayRate);

		// Update replicated version for Simulated Proxies if we are on the server.
		if (IsOwnerActorAuthoritative())
		{
			AnimMontage_UpdateReplicatedDataForMesh(InMesh);
		}
		else
		{
			Server_CurrentMontageSetPlayRateForMesh(InMesh, AnimMontageToSet, InPlayRate);
		}
	}
}

bool UNRAbilitySystemComponent::IsAnimatingAbilityForAnyMesh(UGameplayAbility* Ability) const
{
	for (const FGameplayAbilityLocalAnimMontageForMesh& LocalAnimMontageForMesh: LocalAnimMontageInfoForMeshes)
	{
		if (LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimatingAbility.Get() == Ability)
		{
			return true;
		}
	}

	return false;
}

UGameplayAbility* UNRAbilitySystemComponent::GetAnimatingAbilityForAnyMesh()
{
	for (const FGameplayAbilityLocalAnimMontageForMesh& LocalAnimMontageForMesh: LocalAnimMontageInfoForMeshes)
	{
		if (LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimatingAbility.IsValid())
		{
			return LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimatingAbility.Get();
		}
	}

	return nullptr;
}

TArray<UAnimMontage*> UNRAbilitySystemComponent::GetCurrentMontages() const
{
	TArray<UAnimMontage*> ResMontages;

	for (const FGameplayAbilityLocalAnimMontageForMesh& LocalAnimMontageForMesh: LocalAnimMontageInfoForMeshes)
	{
		UAnimMontage* AnimMontage = LocalAnimMontageForMesh.LocalAnimMontageInfo.AnimMontage;
		UAnimInstance* AnimInstance = IsValid(LocalAnimMontageForMesh.Mesh) && AbilityActorInfo.IsValid() && LocalAnimMontageForMesh.Mesh->GetOwner() == AbilityActorInfo->AvatarActor ? LocalAnimMontageForMesh.Mesh->GetAnimInstance() : nullptr;
		if (AnimMontage && AnimInstance && AnimInstance->Montage_IsActive(AnimMontage))
		{
			ResMontages.Add(AnimMontage);
		}
	}

	return ResMontages;
}

UAnimMontage* UNRAbilitySystemComponent::GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	UAnimMontage* AnimMontage = GetLocalAnimMontageInfoForMesh(InMesh).LocalAnimMontageInfo.AnimMontage;
	if (AnimMontage && AnimInstance && AnimInstance->Montage_IsActive(AnimMontage))
	{
		return AnimMontage;
	}

	return nullptr;
}

int32 UNRAbilitySystemComponent::GetCurrentMontageSectionIDForMesh(USkeletalMeshComponent* InMesh)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	UAnimMontage* CurrentAnimMontage = GetCurrentMontageForMesh(InMesh);
	
	if (CurrentAnimMontage && AnimInstance)
	{
		float MontagePosition = AnimInstance->Montage_GetPosition(CurrentAnimMontage);
		return CurrentAnimMontage->GetSectionIndexFromPosition(MontagePosition);
	}

	return INDEX_NONE;
}

FName UNRAbilitySystemComponent::GetCurrentMontageSectionNameForMesh(USkeletalMeshComponent* InMesh)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	UAnimMontage* CurrentAnimMontage = GetCurrentMontageForMesh(InMesh);

	if (CurrentAnimMontage && AnimInstance)
	{
		float MontagePosition = AnimInstance->Montage_GetPosition(CurrentAnimMontage);
		int32 CurrentSectionID = CurrentAnimMontage->GetSectionIndexFromPosition(MontagePosition);

		return CurrentAnimMontage->GetSectionName(CurrentSectionID);
	}

	return NAME_None;
}

float UNRAbilitySystemComponent::GetCurrentMontageSectionLengthForMesh(USkeletalMeshComponent* InMesh)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	UAnimMontage* CurrentAnimMontage = GetCurrentMontageForMesh(InMesh);
	if (CurrentAnimMontage && AnimInstance)
	{
		int32 CurrentSectionID = GetCurrentMontageSectionIDForMesh(InMesh);
		if (CurrentSectionID != INDEX_NONE)
		{
			TArray<FCompositeSection>& CompositeSections = CurrentAnimMontage->CompositeSections;

			// If we have another section after us, then take delta between both start times.
			if (CurrentSectionID < (CompositeSections.Num() - 1))
			{
				return (CompositeSections[CurrentSectionID + 1].GetTime() - CompositeSections[CurrentSectionID].GetTime());
			}
			// Otherwise we are the last section, so take delta with Montage total time.
			else
			{
				return (CurrentAnimMontage->GetPlayLength() - CompositeSections[CurrentSectionID].GetTime());
			}
		}

		// if we have no sections, just return total length of Montage.
		return CurrentAnimMontage->GetPlayLength();
	}

	return 0.0f;
}

float UNRAbilitySystemComponent::GetCurrentMontageSectionTimeLeftForMesh(USkeletalMeshComponent* InMesh)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	UAnimMontage* CurrentAnimMontage = GetCurrentMontageForMesh(InMesh);
	if (CurrentAnimMontage && AnimInstance && AnimInstance->Montage_IsActive(CurrentAnimMontage))
	{
		const float CurrentPosition = AnimInstance->Montage_GetPosition(CurrentAnimMontage);
		return CurrentAnimMontage->GetSectionTimeLeftFromPos(CurrentPosition);
	}

	return -1.0f;
}

void UNRAbilitySystemComponent::OnPredictiveMontageRejectedForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* PredictiveMontage)
{
	static const float MONTAGE_PREDICTION_REJECT_FADETIME = 0.25f;

	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	if (AnimInstance && PredictiveMontage)
	{
		// If this montage is still playing: kill it
		if (AnimInstance->Montage_IsPlaying(PredictiveMontage))
		{
			AnimInstance->Montage_Stop(MONTAGE_PREDICTION_REJECT_FADETIME, PredictiveMontage);
		}
	}
}

void UNRAbilitySystemComponent::AnimMontage_UpdateReplicatedDataForMesh(USkeletalMeshComponent* InMesh)
{
	check(ShouldRecordMontageReplication())

	AnimMontage_UpdateReplicatedDataForMesh(GetRepAnimMontageInfoForMesh(InMesh));
}

void UNRAbilitySystemComponent::AnimMontage_UpdateReplicatedDataForMesh(FGameplayAbilityRepAnimMontageForMesh& OutRepAnimMontageInfoForMesh)
{
	const UAnimInstance* AnimInstance = IsValid(OutRepAnimMontageInfoForMesh.Mesh) && AbilityActorInfo.IsValid() && OutRepAnimMontageInfoForMesh.Mesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? OutRepAnimMontageInfoForMesh.Mesh->GetAnimInstance() : nullptr;

	const FGameplayAbilityLocalAnimMontageForMesh& LocalAnimMontageInfoForMesh = GetLocalAnimMontageInfoForMesh(OutRepAnimMontageInfoForMesh.Mesh);

	if (AnimInstance && LocalAnimMontageInfoForMesh.LocalAnimMontageInfo.AnimMontage)
	{
		OutRepAnimMontageInfoForMesh.RepAnimMontageInfo.AnimMontage = LocalAnimMontageInfoForMesh.LocalAnimMontageInfo.AnimMontage;

		// Compressed Flags
		const bool bIsStopped = AnimInstance->Montage_GetIsStopped(LocalAnimMontageInfoForMesh.LocalAnimMontageInfo.AnimMontage);

		if (!bIsStopped)
		{
			OutRepAnimMontageInfoForMesh.RepAnimMontageInfo.PlayRate = AnimInstance->Montage_GetPlayRate(LocalAnimMontageInfoForMesh.LocalAnimMontageInfo.AnimMontage);
			OutRepAnimMontageInfoForMesh.RepAnimMontageInfo.Position = AnimInstance->Montage_GetPosition(LocalAnimMontageInfoForMesh.LocalAnimMontageInfo.AnimMontage);
			OutRepAnimMontageInfoForMesh.RepAnimMontageInfo.BlendTime = AnimInstance->Montage_GetBlendTime(LocalAnimMontageInfoForMesh.LocalAnimMontageInfo.AnimMontage);
		}

		if (OutRepAnimMontageInfoForMesh.RepAnimMontageInfo.IsStopped != bIsStopped)
		{
			// Set this prior to calling UpdateShouldTick, so we start ticking if we are playing a Montage
			OutRepAnimMontageInfoForMesh.RepAnimMontageInfo.IsStopped = bIsStopped;

			// When we start or stop an animation, update the clients right away for the Avatar Actor
			if (AbilityActorInfo->AvatarActor != nullptr)
			{
				AbilityActorInfo->AvatarActor->ForceNetUpdate();
			}

			// When this changes, we should update whether or not we should be ticking
			UpdateShouldTick();
		}

		// Replicate NextSectionID to keep it in sync.
		// We actually replicate NextSectionID+1 on a BYTE to put INDEX_NONE in there.
		int32 CurrentSectionID = LocalAnimMontageInfoForMesh.LocalAnimMontageInfo.AnimMontage->GetSectionIndexFromPosition(OutRepAnimMontageInfoForMesh.RepAnimMontageInfo.Position);
		if (CurrentSectionID != INDEX_NONE)
		{
			int32 NextSectionID = AnimInstance->Montage_GetNextSectionID(LocalAnimMontageInfoForMesh.LocalAnimMontageInfo.AnimMontage, CurrentSectionID);
			if (NextSectionID >= (256 - 1))
			{
				ABILITY_LOG( Error, TEXT("AnimMontage_UpdateReplicatedData. NextSectionID = %d.  RepAnimMontageInfo.Position: %.2f, CurrentSectionID: %d. LocalAnimMontageInfo.AnimMontage %s"), 
					NextSectionID, OutRepAnimMontageInfoForMesh.RepAnimMontageInfo.Position, CurrentSectionID, *GetNameSafe(LocalAnimMontageInfo.AnimMontage) );
				ensure(NextSectionID < (256 - 1));
			}
			OutRepAnimMontageInfoForMesh.RepAnimMontageInfo.NextSectionID = uint8(NextSectionID + 1);
		}
		else
		{
			OutRepAnimMontageInfoForMesh.RepAnimMontageInfo.NextSectionID = 0;
		}
	}
}

FGameplayAbilityLocalAnimMontageForMesh& UNRAbilitySystemComponent::GetLocalAnimMontageInfoForMesh(USkeletalMeshComponent* InMesh)
{
	for (FGameplayAbilityLocalAnimMontageForMesh& LocalAnimMontageInfoForMesh: LocalAnimMontageInfoForMeshes)
	{
		if (LocalAnimMontageInfoForMesh.Mesh == InMesh)
		{
			return LocalAnimMontageInfoForMesh;
		}
	}
	
	LocalAnimMontageInfoForMeshes.Add(FGameplayAbilityLocalAnimMontageForMesh(InMesh));
	return LocalAnimMontageInfoForMeshes.Last();
}

FGameplayAbilityRepAnimMontageForMesh& UNRAbilitySystemComponent::GetRepAnimMontageInfoForMesh(USkeletalMeshComponent* InMesh)
{
	for (FGameplayAbilityRepAnimMontageForMesh& RepAnimMontageInfoForMesh: RepAnimMontageInfoForMeshes)
	{
		if (RepAnimMontageInfoForMesh.Mesh == InMesh)
		{
			return RepAnimMontageInfoForMesh;
		}
	}

	RepAnimMontageInfoForMeshes.Add(FGameplayAbilityRepAnimMontageForMesh(InMesh));
	return RepAnimMontageInfoForMeshes.Last();
}

void UNRAbilitySystemComponent::Server_CurrentMontageJumpToSectionNameForMesh_Implementation(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, FName SectionName)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	if (AnimInstance)
	{
		UAnimMontage* CurrentAnimMontage = GetLocalAnimMontageInfoForMesh(InMesh).LocalAnimMontageInfo.AnimMontage;
		if (ClientAnimMontage == CurrentAnimMontage)
		{
			// Set NextSectionName
			AnimInstance->Montage_JumpToSection(SectionName, CurrentAnimMontage);

			// Update replicated version for Simulated Proxies if we are on the server.
			if (IsOwnerActorAuthoritative())
			{
				FGameplayAbilityRepAnimMontageForMesh& RepAnimMontageForMesh = GetRepAnimMontageInfoForMesh(InMesh);

				RepAnimMontageForMesh.RepAnimMontageInfo.SectionIdToPlay = 0;
				if (RepAnimMontageForMesh.RepAnimMontageInfo.AnimMontage && SectionName != NAME_None)
				{
					// we add one so INDEX_NONE can be used in the on rep
					RepAnimMontageForMesh.RepAnimMontageInfo.SectionIdToPlay = RepAnimMontageForMesh.RepAnimMontageInfo.AnimMontage->GetSectionIndex(SectionName) + 1;
				}

				AnimMontage_UpdateReplicatedDataForMesh(InMesh);
			}
		}
	}
}

bool UNRAbilitySystemComponent::Server_CurrentMontageJumpToSectionNameForMesh_Validate(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, FName SectionName)
{
	return true;
}

void UNRAbilitySystemComponent::Server_CurrentMontageSetNextSectionNameForMesh_Implementation(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	if (AnimInstance)
	{
		UAnimMontage* CurrentAnimMontage = GetLocalAnimMontageInfoForMesh(InMesh).LocalAnimMontageInfo.AnimMontage;
		if (ClientAnimMontage == CurrentAnimMontage)
		{
			// Set NextSectionName
			AnimInstance->Montage_SetNextSection(SectionName, NextSectionName, CurrentAnimMontage);

			// Correct position if we are in an invalid section
			float CurrentPosition = AnimInstance->Montage_GetPosition(CurrentAnimMontage);
			int32 CurrentSectionID = CurrentAnimMontage->GetSectionIndexFromPosition(CurrentPosition);
			FName CurrentSectionName = CurrentAnimMontage->GetSectionName(CurrentSectionID);

			int32 ClientSectionID = CurrentAnimMontage->GetSectionIndexFromPosition(ClientPosition);
			FName ClientCurrentSectionName = CurrentAnimMontage->GetSectionName(ClientSectionID);

			if ((CurrentSectionName != ClientCurrentSectionName) || (CurrentSectionName != SectionName))
			{
				// We are in an invalid section, jump to client's position.
				AnimInstance->Montage_SetPosition(CurrentAnimMontage, ClientPosition);
			}

			// Update replicated version for Simulated Proxies if we are on the server.
			if (IsOwnerActorAuthoritative())
			{
				AnimMontage_UpdateReplicatedDataForMesh(InMesh);
			}
		}
	}
}

bool UNRAbilitySystemComponent::Server_CurrentMontageSetNextSectionNameForMesh_Validate(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float ClientPosition, FName SectionName, FName NextSectionName)
{
	return true;
}

void UNRAbilitySystemComponent::Server_CurrentMontageSetPlayRateForMesh_Implementation(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float InPlayRate)
{
	UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo.Get()->AvatarActor ? InMesh->GetAnimInstance() : nullptr;
	if (AnimInstance)
	{
		UAnimMontage* CurrentAnimMontage = GetLocalAnimMontageInfoForMesh(InMesh).LocalAnimMontageInfo.AnimMontage;
		if (ClientAnimMontage == CurrentAnimMontage)
		{
			// Set PlayRate
			AnimInstance->Montage_SetPlayRate(CurrentAnimMontage, InPlayRate);

			// Update replicated version for Simulated Proxies if we are on the server.
			if (IsOwnerActorAuthoritative())
			{
				AnimMontage_UpdateReplicatedDataForMesh(InMesh);
			}
		}
	}
}

bool UNRAbilitySystemComponent::Server_CurrentMontageSetPlayRateForMesh_Validate(USkeletalMeshComponent* InMesh, UAnimMontage* ClientAnimMontage, float InPlayRate)
{
	return true;
}

void UNRAbilitySystemComponent::AbilityLocalInputPressed(int32 InputID)
{
	// Copy From -> Super::AbilityLocalInputPressed(InputID)
	
	// Consume the input if this InputID is overloaded with GenericConfirm/Cancel and the GenericConfim/Cancel callback is bound
	if (IsGenericConfirmInputBound(InputID))
	{
		LocalInputConfirm();
		return;
	}

	if (IsGenericCancelInputBound(InputID))
	{
		LocalInputCancel();
		return;
	}

	// ---------------------------------------------------------

	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.InputID == InputID)
		{
			if (Spec.Ability)
			{
				Spec.InputPressed = true;
				if (Spec.IsActive())
				{
					if (Spec.Ability->bReplicateInputDirectly && IsOwnerActorAuthoritative() == false)
					{
						ServerSetInputPressed(Spec.Handle);
					}

					AbilitySpecInputPressed(Spec);

					// Invoke the InputPressed event. This is not replicated here. If someone is listening, they may replicate the InputPressed event to the server.
					InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, Spec.Handle, Spec.ActivationInfo.GetActivationPredictionKey());					
				}
				else
				{
					//~New Begin 技能是否在输入时激活
					ensureAlwaysMsgf(Cast<UNRGameplayAbility>(Spec.Ability), TEXT("%s 竟然不是NR Ability?"), StringCast<TCHAR>(__FUNCTION__).Get());
					if (const UNRGameplayAbility* GA = Cast<UNRGameplayAbility>(Spec.Ability))
					{
						if (GA && GA->bActivateOnInput)
						{
							TryActivateAbility(Spec.Handle);
						}
					}
					//~New End
					
					//~Old Begin
					// Ability is not active, so try to activate it
					// TryActivateAbility(Spec.Handle);
					//~Old End
				}
			}
		}
	}
}

FGameplayAbilitySpecHandle UNRAbilitySystemComponent::FindAbilitySpecHandleForClass(const TSubclassOf<UGameplayAbility>& AbilityClass)
{
	ABILITYLIST_SCOPE_LOCK()
	for (const FGameplayAbilitySpec& Spec: ActivatableAbilities.Items)
	{
		if (Spec.Ability->GetClass() == AbilityClass)
		{
			return Spec.Handle;
		}
	}

	return FGameplayAbilitySpecHandle();
}

bool UNRAbilitySystemComponent::RestartActiveGameplayEffectDuration(const FActiveGameplayEffectHandle& Handle)
{
	if (!Handle.IsValid())
	{
		return false;
	}

	const FActiveGameplayEffect* Const_AGE = GetActiveGameplayEffect(Handle);
	if (!Const_AGE)
	{
		return false;
	}

	FActiveGameplayEffect* AGE = const_cast<FActiveGameplayEffect*>(Const_AGE);

	// Copy From GameplayEffect.cpp ::RestartActiveGameplayEffectDuration
	AGE->StartServerWorldTime = ActiveGameplayEffects.GetServerWorldTime();
	AGE->CachedStartServerWorldTime = AGE->StartServerWorldTime;
	AGE->StartWorldTime = ActiveGameplayEffects.GetWorldTime();
	ActiveGameplayEffects.MarkItemDirty(*AGE);
	ActiveGameplayEffects.CheckDuration(Handle);
	
	AGE->EventSet.OnTimeChanged.Broadcast(AGE->Handle, AGE->StartWorldTime, AGE->GetDuration());
	OnGameplayEffectDurationChange(*AGE);
	// Copy end

	return true;
}
