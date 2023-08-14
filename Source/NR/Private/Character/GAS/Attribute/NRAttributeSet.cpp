// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Attribute/NRAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UNRAttributeSet::UNRAttributeSet()
{
}

void UNRAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UNRAttributeSet, Health, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAttributeSet, Shield, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAttributeSet, MaxShield, COND_None, REPNOTIFY_Always)
}

void UNRAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	// TODO
}

void UNRAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		SetShield(FMath::Clamp<float>(GetShield(), 0.0f, GetMaxShield()));
	}
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// TODO
		SetShield(FMath::Clamp<float>(GetShield() - GetDamage(), 0.0f, GetMaxShield()));
		SetDamage(0.0f);
	}
}

void UNRAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAttributeSet, Health, OldHealth)
}

void UNRAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAttributeSet, MaxHealth, OldMaxHealth)
}

void UNRAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAttributeSet, Shield, OldShield)
}

void UNRAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAttributeSet, MaxShield, OldMaxShield)
}
