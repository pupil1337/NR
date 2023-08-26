// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "NRAT_WaitInteractTarget.generated.h"

/**
 * 
 */
UCLASS()
class NR_API UNRAT_WaitInteractTarget : public UAbilityTask
{
	GENERATED_BODY()

public:
	UNRAT_WaitInteractTarget();
	
	virtual void TickTask(float DeltaTime) override;

	virtual FString GetDebugString() const override;

//~Begin This Class
	static UNRAT_WaitInteractTarget* WaitInteractTarget(UGameplayAbility* OwningAbility, float Period = 0.1f);

private:
	// Temp
	float CurrTime = 0.0f;
	float LastTraceTime = -999.9f;
	
	float TracePeriod = -1.0f; // <0: no trace; ==0.0f: tick trace; >0.0f: period trace
};
