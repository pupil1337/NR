// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/NRGameplayAbility.h"
#include "NRGA_InteractPassive.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRGA_InteractPassive : public UNRGameplayAbility
{
	GENERATED_BODY()

public:
	UNRGA_InteractPassive();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
