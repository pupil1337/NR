// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "UObject/Object.h"
#include "Engine/StreamableManager.h"
#include "Types/NRWeaponTypes.h"
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

USTRUCT(BlueprintType)
struct FNRCommonSetting
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, DisplayName="交互距离")
	float InteractionDistance = 200.0f;
};

USTRUCT(BlueprintType)
struct FNRCharacterCommonGAS
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, DisplayName="GE 最近受到伤害")
	TSubclassOf<UGameplayEffect> GE_RecentlyDamaged;

	UPROPERTY(EditDefaultsOnly, DisplayName="GE 满护盾")
	TSubclassOf<UGameplayEffect> GE_ShieldFilled;
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

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDataTable> WeaponInformationDataTable;

	UPROPERTY(EditDefaultsOnly)
	TMap<ENRWeaponType, FName> WeaponType2InfoRowName;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNRUserWidgetBase> UIWidgetClass;

	UPROPERTY(EditDefaultsOnly)
	FNRCommonSetting CommonSetting;
	
	UPROPERTY(EditDefaultsOnly)
	FNRCommonVFX CommonVFX;

	UPROPERTY(EditDefaultsOnly)
	FNRCharacterCommonGAS CharacterCommonGAS;
};
