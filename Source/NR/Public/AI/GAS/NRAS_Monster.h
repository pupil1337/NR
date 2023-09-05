// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NRAS_Monster.generated.h"

// Copy From AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)


/**
 * 
 */
UCLASS()
class NR_API UNRAS_Monster : public UAttributeSet
{
	GENERATED_BODY()

public:
	UNRAS_Monster();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	/** Armor */
	UPROPERTY(ReplicatedUsing=OnRep_Armor, BlueprintReadOnly)
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UNRAS_Monster, Armor)

	/** MaxArmor */
	UPROPERTY(ReplicatedUsing=OnRep_MaxArmor, BlueprintReadOnly)
	FGameplayAttributeData MaxArmor;
	ATTRIBUTE_ACCESSORS(UNRAS_Monster, MaxArmor)
	
	/** Shield */
	UPROPERTY(ReplicatedUsing=OnRep_Shield, BlueprintReadOnly)
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UNRAS_Monster, Shield)

	/** MaxShield */
	UPROPERTY(ReplicatedUsing=OnRep_MaxShield, BlueprintReadOnly)
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UNRAS_Monster, MaxShield)
	
	/** Health */
	UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UNRAS_Monster, Health)

	/** MaxHealth */
	UPROPERTY(ReplicatedUsing=OnRep_MaxHealth, BlueprintReadOnly)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UNRAS_Monster, MaxHealth)

	/** Damage (Only On Server) */
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UNRAS_Monster, Damage)

private:

	UFUNCTION()
	void OnRep_Armor(const FGameplayAttributeData& OldArmor);

	UFUNCTION()
	void OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor);
	
	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield);
	
	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield);
	
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
};
