// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/StreamableManager.h"
#include "NRGameSingleton.generated.h"

class UDataTable;

/**
 * 
 */
UCLASS(Blueprintable)
class BASEMODULE_API UNRGameSingleton : public UObject
{
	GENERATED_BODY()

public:
	FStreamableManager StreamableManager;

	static void AddSoftObjectPathToArray(const TSoftObjectPtr<UObject>& SoftObjectPtr, TArray<FSoftObjectPath>& PathArray);

	UPROPERTY(EditDefaultsOnly)
	UDataTable* WeaponInformationDataTable;
};
