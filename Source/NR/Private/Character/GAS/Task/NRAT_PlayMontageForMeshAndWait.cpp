// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Task/NRAT_PlayMontageForMeshAndWait.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "Character/GAS/NRAbilitySystemComponent.h"
#include "GameFramework/Character.h"

UNRAT_PlayMontageForMeshAndWait::UNRAT_PlayMontageForMeshAndWait()
{
	Rate = 1.f;
	bStopWhenAbilityEnds = true;
}

UNRAT_PlayMontageForMeshAndWait* UNRAT_PlayMontageForMeshAndWait::PlayMontageForMeshAndWait(UGameplayAbility* OwningAbility, USkeletalMeshComponent* MeshToPlay, UAnimMontage* MontageToPlay, float Rate, FName StartSection, bool bStopWhenAbilityEnds, float AnimRootMotionTranslationScale, float StartTimeSeconds)
{
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(Rate);

	UNRAT_PlayMontageForMeshAndWait* MyObj = NewAbilityTask<UNRAT_PlayMontageForMeshAndWait>(OwningAbility);
	MyObj->MeshToPlay = MeshToPlay;
	MyObj->MontageToPlay = MontageToPlay;
	MyObj->Rate = Rate;
	MyObj->StartSection = StartSection;
	MyObj->AnimRootMotionTranslationScale = AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = bStopWhenAbilityEnds;
	MyObj->StartTimeSeconds = StartTimeSeconds;

	return MyObj;
}

void UNRAT_PlayMontageForMeshAndWait::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}
	
	bool bPlayedMontage = false;

	if (UNRAbilitySystemComponent* ASC = Cast<UNRAbilitySystemComponent>(AbilitySystemComponent.Get()))
	{
		if (MeshToPlay)
		{
			if (MontageToPlay)
			{
				if (ASC->PlayMontageForMesh(Ability, Ability->GetCurrentActivationInfo(), MeshToPlay, MontageToPlay, Rate, StartSection, StartTimeSeconds) > 0.f)
				{
					// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
					if (ShouldBroadcastAbilityTaskDelegates() == false)
					{
						return;
					}

					// Bind Delegate
					if (UAnimInstance* AnimInstance = MeshToPlay->GetAnimInstance())
					{
						InterruptedHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UNRAT_PlayMontageForMeshAndWait::OnMontageInterrupted);
						
						BlendingOutDelegate.BindUObject(this, &UNRAT_PlayMontageForMeshAndWait::OnMontageBlendingOut);
						AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);
						
						MontageEndedDelegate.BindUObject(this, &UNRAT_PlayMontageForMeshAndWait::OnMontageEnded);
						AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);
					}
					
					ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
					if (Character && (Character->GetLocalRole() == ROLE_Authority ||
									  (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
					{
						Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
					}
					
					bPlayedMontage = true;
				}
				else
				{
					ABILITY_LOG(Warning, TEXT("UNRAT_PlayMontageForMeshAndWait call ASC to PlayMontage failed!"));
				}
			}
			else
			{
				ABILITY_LOG(Warning, TEXT("UNRAT_PlayMontageForMeshAndWait want to play a null montage"));
			}
		}
		else
		{
			ABILITY_LOG(Warning, TEXT("UNRAT_PlayMontageForMeshAndWait want let a null Mesh to play Montage"));
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UNRAT_PlayMontageForMeshAndWait called on invalid NRAbilitySystemComponent"));
	}

	if (!bPlayedMontage)
	{
		ABILITY_LOG(Warning, TEXT("UNRAT_PlayMontageForMeshAndWait called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay),*InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast();
		}
	}

	SetWaitingOnAvatar();
}


void UNRAT_PlayMontageForMeshAndWait::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
	if (Ability && Ability->GetCurrentMontage() == MontageToPlay)
	{
		if (Montage == MontageToPlay)
		{
			if (UNRAbilitySystemComponent* ASC = Cast<UNRAbilitySystemComponent>(AbilitySystemComponent.Get()))
			{
				ASC->ClearAnimatingAbilityForAllMeshes(Ability);
			}

			// Reset AnimRootMotionTranslationScale
			ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
			if (Character && (Character->GetLocalRole() == ROLE_Authority ||
							  (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
			{
				Character->SetAnimRootMotionTranslationScale(1.f);
			}

		}
	}

	if (bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast();
		}
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnBlendOut.Broadcast();
		}
	}
}

void UNRAT_PlayMontageForMeshAndWait::OnMontageInterrupted()
{
	if (StopPlayingMontage())
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast();
		}
	}
}

void UNRAT_PlayMontageForMeshAndWait::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast();
		}
	}

	EndTask();
}

bool UNRAT_PlayMontageForMeshAndWait::StopPlayingMontage()
{
	if (Ability == nullptr)
	{
		return false;
	}

	if (MeshToPlay)
	{
		return false;
	}

	const UAnimInstance* AnimInstance = MeshToPlay->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	UNRAbilitySystemComponent* ASC = Cast<UNRAbilitySystemComponent>(AbilitySystemComponent.Get());
	if (ASC && Ability)
	{
		if (ASC->GetAnimatingAbilityForAnyMesh() == Ability
			&& ASC->GetCurrentMontageForMesh(MeshToPlay) == MontageToPlay)
		{
			// Unbind delegates so they don't get called as well
			if (FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay))
			{
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}

			ASC->CurrentMontageStopForMesh(MeshToPlay);
			return true;
		}
	}

	return false;
}

void UNRAT_PlayMontageForMeshAndWait::ExternalCancel()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast();
	}
	
	Super::ExternalCancel();
}

void UNRAT_PlayMontageForMeshAndWait::OnDestroy(bool AbilityEnded)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(InterruptedHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage();
		}
	}

	Super::OnDestroy(AbilityEnded);
}

FString UNRAT_PlayMontageForMeshAndWait::GetDebugString() const
{
	const UAnimMontage* PlayingMontage = nullptr;
	if (Ability && MeshToPlay)
	{
		const UAnimInstance* AnimInstance = MeshToPlay->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? ToRawPtr(MontageToPlay) : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageAndWait. MeshToPlay: %s MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MeshToPlay), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}
