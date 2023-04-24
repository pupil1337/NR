// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "NRSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class NR_API UNRSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UNRSaveGame();

	UPROPERTY(SaveGame)
	FString PlayerName;

	UPROPERTY(SaveGame)
	FVector Location;
};
