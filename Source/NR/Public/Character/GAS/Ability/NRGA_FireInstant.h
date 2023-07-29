// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/NRGameplayAbility.h"
#include "NRGA_FireInstant.generated.h"

class ANRWeaponBase;
/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRGA_FireInstant : public UNRGameplayAbility
{
	GENERATED_BODY()

public:
	UNRGA_FireInstant();

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
//~Begin This Class
	void FireBullet();

private:
	UFUNCTION()
	void HandleTargetData(const FGameplayAbilityTargetDataHandle& Data);

	void PlayFireMontage();
	
	// Utils
	ANRWeaponBase* GetEquippedWeaponFromActorInfo(const FGameplayAbilityActorInfo* ActorInfo) const;
	bool ResetData(const FGameplayAbilityActorInfo* ActorInfo);

	UPROPERTY(Transient)
	ANRWeaponBase* EquippedWeapon;
};
