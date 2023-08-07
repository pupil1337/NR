// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Types/NRWeaponTypes.h"
#include "NRWeaponSetting.generated.h"

class UNiagaraSystem;

/**
 * 武器属性配置
 */
USTRUCT(BlueprintType)
struct NR_API FNRWeaponSettingRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, DisplayName="开火模式")
	ENRWeaponFireMode FireMode = ENRWeaponFireMode::EWFM_Safe;

	UPROPERTY(EditDefaultsOnly, DisplayName="连发弹药数", meta=(EditCondition="FireMode == ENRWeaponFireMode::EWFM_Burst", ClampMin="0", UIMin="0"))
	uint32 BurstCount = 3;

	UPROPERTY(EditDefaultsOnly, DisplayName="射速", meta=(EditCondition="FireMode != ENRWeaponFireMode::EWFM_Safe", ClampMin="0", UIMin="0", ForceUnits="rpm"))
	uint32 FireRate = 600;

	UPROPERTY(EditDefaultsOnly, DisplayName="子弹类型")
	ENRBulletType BulletType = ENRBulletType::EBT_Max;

	UPROPERTY(EditDefaultsOnly, DisplayName="射线弹伤害", meta=(EditCondition="BulletType == ENRBulletType::EBT_Instant", ClampMin="0", UIMin="0"))
	float InstantDamage = 10.0f;

	UPROPERTY(EditDefaultsOnly, DisplayName="射线弹轨迹", meta=(EditCondition="BulletType == ENRBulletType::EBT_Instant"))
	TSoftObjectPtr<UNiagaraSystem> InstantTraceVFX;
};
