// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_CharacterJump.h"

#include "Character/NRCharacter.h"

UNRGA_CharacterJump::UNRGA_CharacterJump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	
	NRAbilityInputID = ENRAbilityInputID::EAIID_Jump;
}

bool UNRGA_CharacterJump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		if (ActorInfo)
		{
			if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(ActorInfo->AvatarActor.Get()))
			{
				return NRCharacter->CanJump();
			}
		}
	}

	return false;
}

void UNRGA_CharacterJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (ACharacter* Character = Cast<ACharacter>(ActorInfo->AvatarActor.Get()))
	{
		// 虽然设置了Jump技能激活前取消Crouch技能, 但滑铲时角色是Crouching的, 但是不是通过Crouch技能激活的, 所以需要确保在滑铲时跳跃取消角色Crouch
		if (Character->bIsCrouched)
		{
			Character->UnCrouch();	
		}
		Character->Jump();
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
