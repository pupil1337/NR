// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GAS/NRAS_Monster.h"

#include "Net/UnrealNetwork.h"

UNRAS_Monster::UNRAS_Monster()
{
}

void UNRAS_Monster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Monster, Armor,     COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Monster, MaxArmor,  COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Monster, Shield,    COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Monster, MaxShield, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Monster, Health,    COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Monster, MaxHealth, COND_None, REPNOTIFY_Always)
}

void UNRAS_Monster::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UNRAS_Monster::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UNRAS_Monster::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Monster, Armor, OldArmor)
}

void UNRAS_Monster::OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Monster, MaxArmor, OldMaxArmor)
}

void UNRAS_Monster::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Monster, Shield, OldShield)
}

void UNRAS_Monster::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Monster, MaxShield, OldMaxShield)
}

void UNRAS_Monster::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Monster, Health, OldHealth)
}

void UNRAS_Monster::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Monster, MaxHealth, OldMaxHealth)
}
