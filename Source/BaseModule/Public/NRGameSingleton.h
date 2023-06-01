// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Engine/StreamableManager.h"
#include "NRGameSingleton.generated.h"

class UNRUserWidgetBase;
class UDataTable;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct BASEMODULE_API FNRCommonVFX
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, DisplayName="可拾取物特效")
	TSoftObjectPtr<UNiagaraSystem> PickupVFX;
};

/**
 * 
 */
UCLASS(Blueprintable)
class BASEMODULE_API UNRGameSingleton : public UObject
{
	GENERATED_BODY()

public:
	static UNRGameSingleton* Get()
	{
		return GEngine ? Cast<UNRGameSingleton>(GEngine->GameSingleton) : nullptr;
	}
	
	FStreamableManager StreamableManager;

	static void AddSoftObjectPathToArray(const TSoftObjectPtr<UObject>& SoftObjectPtr, TArray<FSoftObjectPath>& PathArray);

	UPROPERTY(EditDefaultsOnly)
	UDataTable* WeaponInformationDataTable;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNRUserWidgetBase> UIWidgetClass;
	
	UPROPERTY(EditDefaultsOnly)
	FNRCommonVFX CommonVFX;
};
