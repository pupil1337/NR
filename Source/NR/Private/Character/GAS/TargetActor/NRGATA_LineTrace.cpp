// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/TargetActor/NRGATA_LineTrace.h"


ANRGATA_LineTrace::ANRGATA_LineTrace()
{
}

void ANRGATA_LineTrace::DoTrace(OUT TArray<FHitResult>& OutHitResults, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params)
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
// Copy From KismetTraceUtils
/** Util for drawing result of multi line trace  */
void DrawDebugLineTraceMulti(const UWorld* World, const FVector& Start, const FVector& End, EDrawDebugTrace::Type DrawDebugType, bool bHit, const TArray<FHitResult>& OutHits, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		// @fixme, draw line with thickness = 2.f?
		if (bHit && OutHits.Last().bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			FVector const BlockingHitPoint = OutHits.Last().ImpactPoint;
			::DrawDebugLine(World, Start, BlockingHitPoint, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, BlockingHitPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}

		// draw hits
		for (int32 HitIdx = 0; HitIdx < OutHits.Num(); ++HitIdx)
		{
			FHitResult const& Hit = OutHits[HitIdx];
			::DrawDebugPoint(World, Hit.ImpactPoint, 16.0f/* KISMET_TRACE_DEBUG_IMPACTPOINT_SIZE */, (Hit.bBlockingHit ? TraceColor.ToFColor(true) : TraceHitColor.ToFColor(true)), bPersistent, LifeTime);
		}
	}
}

void ANRGATA_LineTrace::ShowDebugTrace(const TArray<FHitResult>& HitResults, EDrawDebugTrace::Type DrawDebugType, float Duration /* = 3.0f */)
{
	FVector ViewStart = StartLocation.GetTargetingTransform().GetLocation();
	if (PrimaryPC && bTraceFromPlayerViewPoint)
	{
		FRotator ViewRot;
		PrimaryPC->GetPlayerViewPoint(ViewStart, ViewRot);
	}

	DrawDebugLineTraceMulti(GetWorld(), ViewStart, HitResults[0].TraceEnd, DrawDebugType, true, HitResults, FLinearColor::Green, FLinearColor::Red, Duration);
}
#endif
