// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/NRGameplayAbility.h"
#include "NRGA_InteractPassive.generated.h"

class UAbilityTask_WaitInputPress;
class UAbilityTask_WaitInputRelease;

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

//~Begin This Class
	UFUNCTION()
	void OnLoseInteraction(const FGameplayAbilityTargetDataHandle& Data);
	
	UFUNCTION()
	void OnFindInteraction(const FGameplayAbilityTargetDataHandle& Data);

private:
	UFUNCTION()
	void OnInputPressed(float TimeWaited);

	UFUNCTION()
	void OnInputRelease(float TimeHeld);

	TWeakObjectPtr<AActor> CurrInteraction;
	
	TWeakObjectPtr<UAbilityTask_WaitInputPress> AT_WaitInputPress;
	TWeakObjectPtr<UAbilityTask_WaitInputRelease> AT_WaitInputRelease;
};
