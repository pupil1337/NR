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

	friend class ANRCharacter;
	
public:
	UNRAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UNRAttributeSet, Health)

	UPROPERTY(ReplicatedUsing=OnRep_MaxHealth, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UNRAttributeSet, MaxHealth)

	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
};
