// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "NRMagazineSetting.generated.h"

class UStaticMesh;

/**
 * 武器弹夹配置
 */
USTRUCT(BlueprintType)
struct NR_API FNRMagazineSettingRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, DisplayName="弹夹模型")
	TSoftObjectPtr<UStaticMesh> Mesh;
};
