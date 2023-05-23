// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NRAnimSetting.h"
#include "NRWeaponInformation.generated.h"


USTRUCT(BlueprintType)
struct NR_API FNRWeaponInformationRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="武器名称")
	FText WeaponName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="RowHandle-手臂动画")
	FDataTableRowHandle RowHandle_ArmAnimSet; // FNRArmAnimSetRow

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="RowHandle-全身动画")
	FDataTableRowHandle RowHandle_BodyAnimSet; // FNRBodyAnimSetRow

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
};

