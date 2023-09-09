// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NRAnimSetting.h"
#include "NRWeaponSetting.h"
#include "NRWeaponInformation.generated.h"

class ANRWeaponBase;

USTRUCT(BlueprintType)
struct NR_API FNRWeaponInformationRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, DisplayName="武器名称")
	FText WeaponName;

	UPROPERTY(EditDefaultsOnly, DisplayName="武器类")
	TSoftClassPtr<ANRWeaponBase> WeaponClass;

	UPROPERTY(EditDefaultsOnly, DisplayName="RowHandle-手臂动画")
	FDataTableRowHandle RowHandle_ArmAnimSet; // FNRArmAnimSetRow

	UPROPERTY(EditDefaultsOnly, DisplayName="RowHandle-全身动画")
	FDataTableRowHandle RowHandle_BodyAnimSet; // FNRBodyAnimSetRow

	UPROPERTY(EditDefaultsOnly, DisplayName="RowHandle-武器属性")
	FDataTableRowHandle RowHandle_WeaponSetting; // FNRWeaponSettingRow

	// Getter-手臂动画
	FNRArmAnimSetRow* GetArmAnimSet() const
	{
		if (!RowHandle_ArmAnimSet.IsNull())
		{
			return RowHandle_ArmAnimSet.GetRow<FNRArmAnimSetRow>(TEXT("FNRWeaponInformationRow::GetArmAnimSet()"));
		}
		return nullptr;
	}
	
	// Getter-全身动画
	FNRBodyAnimSetRow* GetBodyAnimSet() const
	{
		if (!RowHandle_ArmAnimSet.IsNull())
		{
			return RowHandle_BodyAnimSet.GetRow<FNRBodyAnimSetRow>(TEXT("FNRWeaponInformationRow::GetBodyAnimSet()"));
		}
		return nullptr;
	}

	// Getter-武器属性
	FNRWeaponSettingRow* GetWeaponSetting() const
	{
		if (!RowHandle_WeaponSetting.IsNull())
		{
			return RowHandle_WeaponSetting.GetRow<FNRWeaponSettingRow>(TEXT("FNRWeaponInformationRow::GetWeaponSetting()"));
		}
		return nullptr;
	}
};

