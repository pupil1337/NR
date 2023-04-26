// Fill out your copyright notice in the Description page of Project Settings.


#include "NRLobbyCharacter.h"


ANRLobbyCharacter::ANRLobbyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ANRLobbyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

