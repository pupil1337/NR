// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_CharacterRun.h"

#include "Character/NRCharacter.h"
#include "Character/NRCharacterMovementComponent.h"

UNRGA_CharacterRun::UNRGA_CharacterRun()
{
	AbilityInputID = ENRAbilityInputID::EAIID_Run;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool UNRGA_CharacterRun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		// TODO
		return true;
	}

	return false;
}

void UNRGA_CharacterRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}

		if (const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			if (UNRCharacterMovementComponent* NRCharacterMovementComponent = Character->GetCharacterMovement<UNRCharacterMovementComponent>())
			{
				NRCharacterMovementComponent->Run(true);
			}
		}
	}
}

void UNRGA_CharacterRun::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UNRGA_CharacterRun::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UNRGA_CharacterRun::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
	}
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	if (ActorInfo)
	{
		if (const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			if (UNRCharacterMovementComponent* NRCharacterMovementComponent = Character->GetCharacterMovement<UNRCharacterMovementComponent>())
			{
				NRCharacterMovementComponent->Run(false);
			}
		}	
	}
}
