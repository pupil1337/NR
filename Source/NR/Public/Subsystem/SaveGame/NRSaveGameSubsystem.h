// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Kismet/GameplayStatics.h"
#include "NRSaveGameSubsystem.generated.h"

class APlayerController;

/**
 * 
 */
UCLASS()
class NR_API UNRSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	void SaveGame(const FAsyncSaveGameToSlotDelegate& SavedDelegate);

	void LoadGame(const FAsyncLoadGameFromSlotDelegate& LoadedDelegate);

private:
	APlayerController* GetLocallyPlayerController() const;
};
