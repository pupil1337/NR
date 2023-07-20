// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/NRGameplayAbility.h"
#include "NRGA_Shield.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRGA_Shield : public UNRGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ShieldInit;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> ShieldRegen;
	
public:
	UNRGA_Shield();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
