// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRAnimSetting.h"
#include "Engine/DataTable.h"
#include "NRWeaponInformation.generated.h"


USTRUCT(BlueprintType)
struct NR_API FNRWeaponInformationRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="武器名称")
	FText WeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="动画设置-RowHandle")
	FDataTableRowHandle RowHandle_AnimSetting; // FNRAnimSettingRow

	// Getter
	FNRAnimSettingRow* GetAnimSetting() const
	{
		if (!RowHandle_AnimSetting.IsNull())
		{
			return RowHandle_AnimSetting.GetRow<FNRAnimSettingRow>(TEXT("FNRWeaponInformationRow::GetAnimSetting()"));
		}
		return nullptr;
	}
};

