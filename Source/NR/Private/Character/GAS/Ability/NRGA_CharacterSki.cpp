// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_CharacterSki.h"

#include "Character/NRCharacterMovementComponent.h"
#include "GameFramework/Character.h"

UNRGA_CharacterSki::UNRGA_CharacterSki()
{
	NRAbilityInputID = ENRAbilityInputID::EAIID_Ski;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
}

bool UNRGA_CharacterSki::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		// TODO
		return true;
	}
	
	return false;
}

void UNRGA_CharacterSki::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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
				NRCharacterMovementComponent->Ski(true);
			}
		}
	}
}

void UNRGA_CharacterSki::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

void UNRGA_CharacterSki::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UNRGA_CharacterSki::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
	}
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	if (ActorInfo)
	{
		if (const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			if (UNRCharacterMovementComponent* NRCharacterMovementComponent = Character->GetCharacterMovement<UNRCharacterMovementComponent>())
			{
				NRCharacterMovementComponent->Ski(false);
			}
		}
	}
}
