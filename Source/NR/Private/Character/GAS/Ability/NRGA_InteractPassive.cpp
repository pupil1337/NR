// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_InteractPassive.h"

#include "Character/GAS/Task/NRAT_WaitInteractTarget.h"

UNRGA_InteractPassive::UNRGA_InteractPassive()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	bAutoActiveOnGive = true;
}

void UNRGA_InteractPassive::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//FGameplayAbilityTargetingLocationInfo StartLocation;
	//StartLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform
	//UNRAT_WaitInteractTarget* AT_WaitInteractTarget = UNRAT_WaitInteractTarget::WaitInteractTarget(this);
	//->ReadyForActivation();
}
