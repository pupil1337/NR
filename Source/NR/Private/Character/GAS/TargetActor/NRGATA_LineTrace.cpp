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
