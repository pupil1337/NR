// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "NRMMC_ShieldRegen.generated.h"

/**
 * 护盾回复：10%每秒 (周期0.1s 每次回复1%) CalculateBaseMagnitude最终结果乘以Coefficient实现升级
 */
UCLASS()
class NR_API UNRMMC_ShieldRegen : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UNRMMC_ShieldRegen();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

//~Begin This Class
private:
	FGameplayEffectAttributeCaptureDefinition ShieldDef;
	FGameplayEffectAttributeCaptureDefinition MaxShieldDef;
};
