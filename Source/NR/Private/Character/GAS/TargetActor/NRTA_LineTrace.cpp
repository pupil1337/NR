// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/TargetActor/NRTA_LineTrace.h"


ANRTA_LineTrace::ANRTA_LineTrace()
{
}

void ANRTA_LineTrace::DoTrace(OUT TArray<FHitResult>& OutHitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params)
{
	check(World);
	
	TArray<FHitResult> HitResults;
	World->LineTraceMultiByProfile(HitResults, Start, End, ProfileName, Params);

	for (int32 HitIdx = 0; HitIdx < HitResults.Num(); ++HitIdx)
	{
		FHitResult& Hit = HitResults[HitIdx];

		if (!Hit.GetActor() || FilterHandle.FilterPassesForActor(Hit.GetActor()))
		{
			Hit.TraceStart = Start;
			Hit.TraceEnd = End;
			
			OutHitResults.Add(Hit);
		}
	}
}

#if ENABLE_DRAW_DEBUG
// Reference KismetTraceUtils
void DrawDebugLineTraceMulti(const UWorld* World, const FVector& Start, const FVector& End, const TArray<FHitResult>& OutHits)
{
	constexpr float LifeTime = 3.0f;
	
	// 画线
	// @fixme, draw line with thickness = 2.f?
	if (OutHits.Last().bBlockingHit)
	{
		// blockHit前为绿色, 之后是红色
		FVector const BlockingHitPoint = OutHits.Last().ImpactPoint;
		::DrawDebugLine(World, Start, BlockingHitPoint, FColor::Green, false, LifeTime);
		::DrawDebugLine(World, BlockingHitPoint, End, FColor::Red, false, LifeTime);
	}
	else
	{
		// 没有blockHit则为全绿色
		::DrawDebugLine(World, Start, End, FColor::Green, false, LifeTime);
	}

	// 画点
	for (int32 HitIdx = 0; HitIdx < OutHits.Num(); ++HitIdx)
	{
		const FHitResult& Hit = OutHits[HitIdx];
		::DrawDebugPoint(World, Hit.ImpactPoint, 16.0f, Hit.bBlockingHit ? FColor::Red : FColor::Green, false, 3.0f);
	}
}

void ANRTA_LineTrace::ShowDebugTrace(const TArray<FHitResult>& HitResults)
{
	FVector ViewStart = StartLocation.GetTargetingTransform().GetLocation();
	if (PrimaryPC && bTraceFromPlayerViewPoint)
	{
		FRotator ViewRot;
		PrimaryPC->GetPlayerViewPoint(ViewStart, ViewRot);
	}

	DrawDebugLineTraceMulti(GetWorld(), ViewStart, HitResults[0].TraceEnd, HitResults);
}
#endif
