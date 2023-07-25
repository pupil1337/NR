// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/NRGameplayAbility.h"
#include "NRGA_Fire.generated.h"

class UNRGA_FireInstant;
class ANRWeaponBase;
class UNRAbilitySystemComponent;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRGA_Fire : public UNRGameplayAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNRGA_FireInstant> NRGA_FireInstantClass;
	
public:
	UNRGA_Fire();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
	
//~Begin This Class
private:
	UFUNCTION()
	virtual void Automatic_Fire();
	
	// Utils
	ANRWeaponBase* GetEquippedWeaponFromActorInfo(const FGameplayAbilityActorInfo* ActorInfo) const;
	bool ResetData(const FGameplayAbilityActorInfo* ActorInfo);
	bool IsRatePassed(uint32 FireRate) const;
	
	// Temp
	UPROPERTY(Transient)
	ANRWeaponBase* EquippedWeapon;

	UPROPERTY(Transient)
	UNRAbilitySystemComponent* NRAbilitySystemComponent;
	FGameplayAbilitySpecHandle GA_FireInstantSpecHandle;
	UPROPERTY(Transient)
	UNRGA_FireInstant* GA_FireInstant;
	
	float PreFireTime;
};
