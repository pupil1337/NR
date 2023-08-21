// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "NRExecCalc_Monster.generated.h"

/**
 * 
 */
UCLASS()
class NR_API UNRExecCalc_Monster : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UNRExecCalc_Monster();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
