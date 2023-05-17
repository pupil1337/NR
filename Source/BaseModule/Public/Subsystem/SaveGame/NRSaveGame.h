// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "NRSaveGame.generated.h"

USTRUCT()
struct FSaveGameData
{
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	FVector Location = FVector::ZeroVector;
};

/**
 * 
 */
UCLASS()
class BASEMODULE_API UNRSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UNRSaveGame();

//~Begin This Class
	UPROPERTY(SaveGame)
	FSaveGameData SaveGameData;
};
