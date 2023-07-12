// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_CharacterJump.h"

#include "Character/NRCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UNRGA_CharacterJump::UNRGA_CharacterJump()
{
	AbilityInputID = ENRAbilityInputID::EAIID_Jump;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::Type::NonInstanced;
}

// bool UNRGA_CharacterJump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
// {
// 	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
// 	{
// 		// if (ActorInfo)
// 		// {
// 		// 	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(ActorInfo->AvatarActor.Get()))
// 		// 	{
// 		// 		return NRCharacter->CanJump();
// 		// 	}
// 		// }
// 		// TODO
// 		return true;
// 	}
//
// 	return false;
// }

void UNRGA_CharacterJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}

		if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			Character->Jump();
		}
	}
}

void UNRGA_CharacterJump::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UNRGA_CharacterJump::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UNRGA_CharacterJump::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
	}
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	if (ActorInfo)
	{
		if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			Character->StopJumping();
		}	
	}
}
