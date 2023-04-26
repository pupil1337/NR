// Fill out your copyright notice in the Description page of Project Settings.


#include "NRLobbyGameMode.h"


ANRLobbyGameMode::ANRLobbyGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void ANRLobbyGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}

