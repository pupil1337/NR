// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/NREngineSubsystem.h"

#include "AbilitySystemGlobals.h"

void UNREngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UAbilitySystemGlobals::Get().InitGlobalData();
}
