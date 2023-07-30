// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRGATA_Trace.h"
#include "NRGATA_LineTrace.generated.h"

UCLASS()
class NR_API ANRGATA_LineTrace : public ANRGATA_Trace
{
	GENERATED_BODY()

public:
	ANRGATA_LineTrace();

protected:
	virtual void DoTrace(OUT TArray<FHitResult>& OutHitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params) override;
};
