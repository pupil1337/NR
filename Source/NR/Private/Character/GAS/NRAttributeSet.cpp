// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/NRAttributeSet.h"

#include "Net/UnrealNetwork.h"

UNRAttributeSet::UNRAttributeSet()
{
}

void UNRAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UNRAttributeSet, Health, COND_None, REPNOTIFY_Always)
}

void UNRAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAttributeSet, Health, OldHealth)
}

void UNRAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAttributeSet, MaxHealth, OldMaxHealth)
}
