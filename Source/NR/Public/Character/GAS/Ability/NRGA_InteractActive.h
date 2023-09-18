// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/NRGameplayAbility.h"
#include "NRGA_InteractActive.generated.h"

class UAbilityTask_WaitInputRelease;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRGA_InteractActive : public UNRGameplayAbility
{
	GENERATED_BODY()

public:
	UNRGA_InteractActive();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

//~Begin This Class
private:
	UFUNCTION()
	void OnInputRelease(float TimeHeld);
	
	TWeakObjectPtr<UAbilityTask_WaitInputRelease> AT_WaitInputRelease;
};
