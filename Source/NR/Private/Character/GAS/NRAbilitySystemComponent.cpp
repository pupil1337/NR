// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/NRAbilitySystemComponent.h"

#include "Character/GAS/NRGameplayAbility.h"


float UNRAbilitySystemComponent::PlayMontageForMesh(UNRGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, USkeletalMeshComponent* InMesh, UAnimMontage* NewMontage, float InPlayRate, FName StartSectionName, float StartTimeSeconds)
{
	float Duration = -1.0f;

	if (NewMontage)
	{
		if (UAnimInstance* AnimInstance = IsValid(InMesh) && AbilityActorInfo.IsValid() && InMesh->GetOwner() == AbilityActorInfo->AvatarActor ? InMesh->GetAnimInstance() : nullptr)
		{
			Duration = AnimInstance->Montage_Play(NewMontage, InPlayRate, EMontagePlayReturnType::MontageLength, StartTimeSeconds);
			if (Duration > 0.0f)
			{
				FGameplayAbilityLocalAnimMontageForMesh& AnimMontageInfo = GetLocalAnimMontageInfoForMesh(InMesh);
				if (AnimMontageInfo.LocalAnimMontageInfo.AnimatingAbility.IsValid() && AnimMontageInfo.LocalAnimMontageInfo.AnimatingAbility.Get() != AnimatingAbility)
				{
					// The ability that was previously animating will have already gotten the 'interrupted' callback.
					// It may be a good idea to make this a global policy and 'cancel' the ability.
					// 
					// For now, we expect it to end itself when this happens.
				}

				if (NewMontage->HasRootMotion() && AnimInstance->GetOwningActor())
				{
					UE_LOG(LogRootMotion, Log, TEXT("UAbilitySystemComponent::PlayMontage %s, Role: %s")
						, *GetNameSafe(NewMontage)
						, *UEnum::GetValueAsString(TEXT("Engine.ENetRole"), AnimInstance->GetOwningActor()->GetLocalRole())
					);
				}

				AnimMontageInfo.LocalAnimMontageInfo.AnimMontage = NewMontage;
				AnimMontageInfo.LocalAnimMontageInfo.AnimatingAbility = AnimatingAbility;
				AnimMontageInfo.LocalAnimMontageInfo.PlayInstanceId = (AnimMontageInfo.LocalAnimMontageInfo.PlayInstanceId < UINT8_MAX ? AnimMontageInfo.LocalAnimMontageInfo.PlayInstanceId + 1 : 0);

				if (AnimatingAbility)
				{
					AnimatingAbility->SetCurrentAbilityMeshMontage(InMesh, NewMontage);
				}
			
				// Start at a given Section.
				if (StartSectionName != NAME_None)
				{
					AnimInstance->Montage_JumpToSection(StartSectionName, NewMontage);
				}
				
				// Replicate for non-owners and for replay recordings
				// The data we set from GetRepAnimMontageInfo_Mutable() is used both by the server to replicate to clients and by clients to record replays.
				// We need to set this data for recording clients because there exists network configurations where an abilities montage data will not replicate to some clients (for example: if the client is an autonomous proxy.)
				if (ShouldRecordMontageReplication())
				{
					FGameplayAbilityRepAnimMontageForMesh& RepAnimMontageInfo = GetRepAnimMontageInfoForMesh(InMesh);

					// Those are static parameters, they are only set when the montage is played. They are not changed after that.
					RepAnimMontageInfo.RepAnimMontageInfo.AnimMontage = NewMontage;
					RepAnimMontageInfo.RepAnimMontageInfo.PlayInstanceId = (RepAnimMontageInfo.RepAnimMontageInfo.PlayInstanceId < UINT8_MAX ? RepAnimMontageInfo.RepAnimMontageInfo.PlayInstanceId + 1 : 0);

					RepAnimMontageInfo.RepAnimMontageInfo.SectionIdToPlay = 0;
					if (RepAnimMontageInfo.RepAnimMontageInfo.AnimMontage && StartSectionName != NAME_None)
					{
						// we add one so INDEX_NONE can be used in the on rep
						RepAnimMontageInfo.RepAnimMontageInfo.SectionIdToPlay = RepAnimMontageInfo.RepAnimMontageInfo.AnimMontage->GetSectionIndex(StartSectionName) + 1;
					}

					// Update parameters that change during Montage life time.
					AnimMontage_UpdateReplicatedData(); // TODO
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
						PredictionKey.NewRejectedDelegate().BindUObject(this, &UNRAbilitySystemComponent::OnPredictiveMontageRejectedForMesh, InMesh, NewMontage);
					}
				}
			}
		}	
	}

	return Duration;
}

void UNRAbilitySystemComponent::OnRep_ReplicatedAnimMontageForMeshes()
{
}

float UNRAbilitySystemComponent::PlayMontageSimulatedForMesh(USkeletalMeshComponent* Mesh, UAnimMontage* Montage, float InPlayRate, FName StartSectionName)
{
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

FGameplayAbilityLocalAnimMontageForMesh& UNRAbilitySystemComponent::GetLocalAnimMontageInfoForMesh(USkeletalMeshComponent* InMesh)
{
	for (FGameplayAbilityLocalAnimMontageForMesh& MontageInfo: LocalAnimMontageInfoForMeshes)
	{
		if (MontageInfo.Mesh == InMesh)
		{
			return MontageInfo;
		}
	}
	
	LocalAnimMontageInfoForMeshes.Add(FGameplayAbilityLocalAnimMontageForMesh(InMesh));
	return LocalAnimMontageInfoForMeshes.Last();
}

FGameplayAbilityRepAnimMontageForMesh& UNRAbilitySystemComponent::GetRepAnimMontageInfoForMesh(USkeletalMeshComponent* InMesh)
{
	for (FGameplayAbilityRepAnimMontageForMesh& MontageInfo: RepAnimMontageInfoForMeshes)
	{
		if (MontageInfo.Mesh == InMesh)
		{
			return MontageInfo;
		}
	}

	RepAnimMontageInfoForMeshes.Add(FGameplayAbilityRepAnimMontageForMesh(InMesh));
	return RepAnimMontageInfoForMeshes.Last();
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
