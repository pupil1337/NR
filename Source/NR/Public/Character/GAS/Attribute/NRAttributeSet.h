// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "NRAttributeSet.generated.h"


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
class NR_API UNRAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UNRAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	/** Health */
	UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UNRAttributeSet, Health)

	/** MaxHealth */
	UPROPERTY(ReplicatedUsing=OnRep_MaxHealth, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UNRAttributeSet, MaxHealth)

	/** Shield */
	UPROPERTY(ReplicatedUsing=OnRep_Shield, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Shield;
	ATTRIBUTE_ACCESSORS(UNRAttributeSet, Shield)

	UPROPERTY(ReplicatedUsing=OnRep_MaxShield, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MaxShield;
	ATTRIBUTE_ACCESSORS(UNRAttributeSet, MaxShield)

	/** Damage (Only In Server. Not Replicated)*/
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UNRAttributeSet, Damage)

private:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_Shield(const FGameplayAttributeData& OldShield);

	UFUNCTION()
	void OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield);
	
};
