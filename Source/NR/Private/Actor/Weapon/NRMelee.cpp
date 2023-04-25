// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/NRMelee.h"


// Sets default values
ANRMelee::ANRMelee()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ANRMelee::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ANRMelee::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

