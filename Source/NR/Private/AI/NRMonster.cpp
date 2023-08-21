// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/NRMonster.h"

#include "AbilitySystemComponent.h"
#include "AI/GAS/NRAS_Monster.h"


ANRMonster::ANRMonster()
{
	PrimaryActorTick.bCanEverTick = false;

	// AbilitySystemComponent
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("技能系统"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// AS_Monster
	AS_Monster = CreateDefaultSubobject<UNRAS_Monster>(TEXT("属性集"));
}

void ANRMonster::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

