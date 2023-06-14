// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Types/NRWeaponTypes.h"
#include "NRWeaponSetting.generated.h"

/**
 * 武器属性配置
 */
USTRUCT(BlueprintType)
struct NR_API FNRWeaponSettingRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, DisplayName="开火模式")
	ENRWeaponFireMode FireMode;

	UPROPERTY(EditDefaultsOnly, DisplayName="连发弹药数", meta=(EditCondition="FireMode == ENRWeaponFireMode::EWFM_Burst", ClampMin="0", UIMin="0"))
	uint32 BurstCount;

	UPROPERTY(EditDefaultsOnly, DisplayName="射速", meta=(ClampMin="0", UIMin="0", ForceUnits="rpm"))
	uint32 FireRate;
};
