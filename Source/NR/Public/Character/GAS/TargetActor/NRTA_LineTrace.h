// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRTA_Trace.h"
#include "NRTA_LineTrace.generated.h"

UCLASS()
class NR_API ANRTA_LineTrace : public ANRTA_Trace
{
	GENERATED_BODY()

public:
	ANRTA_LineTrace();

protected:
	virtual void DoTrace(OUT TArray<FHitResult>& OutHitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params) override;

#if ENABLE_DRAW_DEBUG
	virtual void ShowDebugTrace(const TArray<FHitResult>& HitResults) override;
#endif
};
