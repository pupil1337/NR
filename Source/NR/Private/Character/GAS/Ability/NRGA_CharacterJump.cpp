﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_CharacterJump.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UNRGA_CharacterJump::UNRGA_CharacterJump()
{
	AbilityInputID = ENRAbilityInputID::EAIID_Jump;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::Type::NonInstanced;
}

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
			if (const UCharacterMovementComponent* CharacterMovementComponent = Character->GetCharacterMovement())
			{
				// TODO 改成ability
				if (CharacterMovementComponent->IsCrouching())
				{
					Character->UnCrouch();
				}
			}
			Character->Jump();
		}
	}
}
