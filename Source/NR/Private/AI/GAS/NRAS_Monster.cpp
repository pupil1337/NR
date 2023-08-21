// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GAS/NRAS_Monster.h"

#include "Net/UnrealNetwork.h"

UNRAS_Monster::UNRAS_Monster()
{
}

void UNRAS_Monster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Monster, Health, COND_None, REPNOTIFY_Always)
}

void UNRAS_Monster::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UNRAS_Monster::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
}

void UNRAS_Monster::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Monster, Health, OldHealth)
}
