// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_Fire.h"

#include "AbilitySystemGlobals.h"

UNRGA_Fire::UNRGA_Fire()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	NRAbilityInputID = ENRAbilityInputID::EAIID_Fire;
}

bool UNRGA_Fire::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UNRGA_Fire::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	float TickFrequency = 1.0f / 100.0f; // 每秒100发
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Duration(TickFrequency);
	GetWorld()->GetTimerManager().SetTimer(TickTimerHandle, this, &ThisClass::FiringTick, TickFrequency, true);
}

void UNRGA_Fire::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	CancelAbility(Handle, ActorInfo, ActivationInfo, false);
}

void UNRGA_Fire::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	GetWorld()->GetTimerManager().ClearTimer(TickTimerHandle);
	
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UNRGA_Fire::FiringTick()
{
	
}
