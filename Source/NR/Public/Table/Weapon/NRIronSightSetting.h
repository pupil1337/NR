// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NRIronSightSetting.generated.h"

class UStaticMesh;

/**
 * 武器机瞄配置
 */
USTRUCT(BlueprintType)
struct NR_API FNRIronSightSettingRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, DisplayName="机瞄模型")
	TSoftObjectPtr<UStaticMesh> Mesh;
};
