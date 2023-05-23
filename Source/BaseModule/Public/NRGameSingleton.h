// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/StreamableManager.h"
#include "NRGameSingleton.generated.h"

/**
 * 
 */
UCLASS()
class BASEMODULE_API UNRGameSingleton : public UObject
{
	GENERATED_BODY()

public:
	FStreamableManager StreamableManager;

	static void AddSoftObjectPathToArray(const TSoftObjectPtr<UObject>& SoftObjectPtr, TArray<FSoftObjectPath>& PathArray);
};
