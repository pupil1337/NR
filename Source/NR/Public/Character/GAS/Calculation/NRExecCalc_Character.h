// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "NRExecCalc_Character.generated.h"

/**
 * 
 */
UCLASS()
class NR_API UNRExecCalc_Character : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UNRExecCalc_Character();
	
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
