﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "NRStatics.h"

#include "Kismet/GameplayStatics.h"
#include "Types/NRWeaponTypes.h"


void UNRStatics::AddSoftObjectPathToArray(const TSoftObjectPtr<>& SoftObjectPtr, TArray<FSoftObjectPath>& PathArray)
{
	if (SoftObjectPtr.IsPending())
	{
		PathArray.AddUnique(SoftObjectPtr.ToSoftObjectPath());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Assets: %s加载失败. [1]a live UObject:%d, [2]a Object:%d"), *SoftObjectPtr.GetAssetName(), SoftObjectPtr.IsValid(), SoftObjectPtr.IsNull())
	}
}

void UNRStatics::SetFPS_SeparateFOV(USkeletalMeshComponent* Mesh, bool bEnable, bool bSeparate)
{
	if (Mesh)
	{
		const float SeparateFOVAlpha = bEnable ? 1.0f : 0.0f;
		const float SeparateAlpha = bSeparate ? 0.1f : 1.0f;
		Mesh->SetScalarParameterValueOnMaterials(NAME_Separate_FOV_Alpha, SeparateFOVAlpha);
		Mesh->SetScalarParameterValueOnMaterials(NAME_Separate_Alpha, SeparateAlpha);
		Mesh->SetCastShadow(false/* !bSeparate TODO:引擎有bug bSelfShadowOnly暂不可用 */);
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
