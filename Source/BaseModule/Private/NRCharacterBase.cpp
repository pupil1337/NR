// Fill out your copyright notice in the Description page of Project Settings.


#include "NRCharacterBase.h"


ANRCharacterBase::ANRCharacterBase(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ANRCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

