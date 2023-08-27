// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NRAS_Character.generated.h"


// copy from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class NR_API UNRAS_Character : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UNRAS_Character();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	/** Health */
	UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UNRAS_Character, Health)

	/** MaxHealth */
	UPROPERTY(ReplicatedUsing=OnRep_MaxHealth, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UNRAS_Character, MaxHealth)

	/** Shield */
	UPROPERTY(ReplicatedUsing=OnRep_Shield, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UNRAS_Character, Shield)

	UPROPERTY(ReplicatedUsing=OnRep_MaxShield, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UNRAS_Character, MaxShield)

	/** Damage (Only In Server. Not Replicated) */
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UNRAS_Character, Damage)

	/** RecentlyDamaged ActiveEffectHandle (Only In Server) */
	FActiveGameplayEffectHandle AGE_RecentlyDamagedHandle;

private:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield);

	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield);

	FTimerHandle RecentlyDamagedHandle;
};
