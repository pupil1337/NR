// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/TargetActor/NRGATA_Trace.h"


// Sets default values
ANRGATA_Trace::ANRGATA_Trace()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANRGATA_Trace::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANRGATA_Trace::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANRGATA_Trace::StopTargeting()
{
	// TODO
}

