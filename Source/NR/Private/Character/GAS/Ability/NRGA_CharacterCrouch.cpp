// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_CharacterCrouch.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UNRGA_CharacterCrouch::UNRGA_CharacterCrouch()
{
	AbilityInputID = ENRAbilityInputID::EAIID_Crouch;
}

bool UNRGA_CharacterCrouch::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		if (const ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			if (const UCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovement())
			{
				return !CharacterMovementComponent->IsFalling();
			}
		}
	}
	return false;
}

void UNRGA_CharacterCrouch::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}

		if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
		{
			Character->Crouch();
		}
	}
}
