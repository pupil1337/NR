﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Kismet/GameplayStatics.h"
#include "NRSaveGameSubsystem.generated.h"

class APlayerController;
class UNRSaveGame;

/**
 * 
 */
UCLASS()
class BASEMODULE_API UNRSaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
// ~Begin This Class
public:
	static bool HaveSavedGame();
	void SaveGame(const FAsyncSaveGameToSlotDelegate& SavedDelegate) const;
	static void LoadGame(const FAsyncLoadGameFromSlotDelegate& LoadedDelegate);

private:
	APlayerController* GetLocallyPlayerController() const;
};
