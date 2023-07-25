// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_Fire.h"

#include "AbilitySystemGlobals.h"
#include "Abilities/Tasks/AbilityTask_WaitDelay.h"
#include "Character/NRCharacter.h"
#include "Character/Component/NRCombatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/GAS/NRAbilitySystemComponent.h"
#include "Character/GAS/Ability/NRGA_FireInstant.h"
#include "Table/Weapon/NRWeaponSetting.h"

UNRGA_Fire::UNRGA_Fire()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	NRAbilityInputID = ENRAbilityInputID::EAIID_Fire;
}

bool UNRGA_Fire::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return GetEquippedWeaponFromActorInfo(ActorInfo) != nullptr;
	}

	return false;
}

void UNRGA_Fire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ResetData(ActorInfo))
	{
		// Automatic
		if (EquippedWeapon->GetWeaponSettingRow()->FireMode == ENRWeaponFireMode::EWFM_Automatic)
		{
			NRAbilitySystemComponent->TryActivateAbility(GA_FireInstantSpecHandle);

			UAbilityTask_WaitDelay* AT_WaitDelay = UAbilityTask_WaitDelay::WaitDelay(GA_FireInstant, 60.0f / EquippedWeapon->GetWeaponSettingRow()->FireRate);
			AT_WaitDelay->OnFinish.AddDynamic(this, &ThisClass::Automatic_Fire);
			AT_WaitDelay->ReadyForActivation();
		}
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);	
	}
}

void UNRGA_Fire::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	CancelAbility(Handle, ActorInfo, ActivationInfo, false);
}

void UNRGA_Fire::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (GA_FireInstant)
	{
		NRAbilitySystemComponent->CancelAbilityHandle(GA_FireInstantSpecHandle);
	}
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UNRGA_Fire::Automatic_Fire()
{
	if (EquippedWeapon && GA_FireInstant)
	{
		GA_FireInstant->FireBullet();
		
		UAbilityTask_WaitDelay* AT_WaitDelay = UAbilityTask_WaitDelay::WaitDelay(GA_FireInstant, 60.0f / EquippedWeapon->GetWeaponSettingRow()->FireRate);
		AT_WaitDelay->OnFinish.AddDynamic(this, &ThisClass::Automatic_Fire);
		AT_WaitDelay->ReadyForActivation();
	}
}

// Utils ==============================================================================================
ANRWeaponBase* UNRGA_Fire::GetEquippedWeaponFromActorInfo(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (UNRCombatComponent* CombatComponent = NRCharacter->GetComponentByClass<UNRCombatComponent>())
		{
			return CombatComponent->GetEquippedWeapon();
		}
	}
	
	return nullptr;
}

bool UNRGA_Fire::ResetData(const FGameplayAbilityActorInfo* ActorInfo)
{
	// 设置当前装备的武器
	EquippedWeapon = GetEquippedWeaponFromActorInfo(ActorInfo);
	if (EquippedWeapon)
	{
		// FireInstant
		NRAbilitySystemComponent = Cast<UNRAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
		if (NRAbilitySystemComponent)
		{
			GA_FireInstantSpecHandle = NRAbilitySystemComponent->FindAbilitySpecHandleForClass(NRGA_FireInstantClass);
			if (const FGameplayAbilitySpec* AbilitySpec = NRAbilitySystemComponent->FindAbilitySpecFromHandle(GA_FireInstantSpecHandle))
			{
				GA_FireInstant = Cast<UNRGA_FireInstant>(AbilitySpec->GetPrimaryInstance());
				if (GA_FireInstant)
				{
					return true;
				}
			}
		}
	}

	return false;
}

bool UNRGA_Fire::IsRatePassed(uint32 FireRate) const
{
	return PreFireTime + 60.0f / FireRate <= UGameplayStatics::GetTimeSeconds(this);
}
