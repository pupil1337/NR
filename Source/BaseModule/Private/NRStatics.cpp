// Fill out your copyright notice in the Description page of Project Settings.


#include "NRStatics.h"

#include "Kismet/GameplayStatics.h"


void UNRStatics::AddSoftObjectPathToArray(const TSoftObjectPtr<>& SoftObjectPtr, TArray<FSoftObjectPath>& PathArray)
{
	if (SoftObjectPtr.IsPending())
	{
		PathArray.AddUnique(SoftObjectPtr.ToSoftObjectPath());
	}
}

bool UNRStatics::CrosshairTrace(const APlayerController* PlayerController, float TraceDistance, FHitResult& HitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	else
	{
		return false;
	}

	FVector WorldPosition, WorldDirection;
	if (UGameplayStatics::DeprojectScreenToWorld(PlayerController, ViewportSize/2.0, WorldPosition, WorldDirection))
	{
		const FVector Start = WorldPosition;
		const FVector End = Start + WorldDirection*TraceDistance;

		if (const UWorld* World = PlayerController->GetWorld())
		{
			// DrawDebugLine(World, Start, End, FColor::Red, false, 3.0);
			World->LineTraceSingleByChannel(
				HitResult,
				Start,
				End,
				ECC_Visibility
			);
			if (!HitResult.bBlockingHit)
			{
				HitResult.ImpactPoint = End;
			}
			return true;
		}
	}
	
	return false;
}
