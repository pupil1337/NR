// Fill out your copyright notice in the Description page of Project Settings.


#include "Static/NRStatics.h"

#include "NRGameSingleton.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Types/NRWeaponTypes.h"

void UNRStatics::RequestAsyncLoad(OUT TSharedPtr<FStreamableHandle>& OutStreamableHandle, const TArray<FSoftObjectPath>& TargetsToStream, const FStreamableDelegate& DelegateToCall, const TAsyncLoadPriority Priority, const bool bManageActiveHandle, const bool bStartStalled, const FString& DebugName)
{
	if (UNRGameSingleton* NRGameSingleton = UNRGameSingleton::Get())
	{
		if (OutStreamableHandle.IsValid())
		{
			OutStreamableHandle->ReleaseHandle();
			OutStreamableHandle.Reset();
		}

		OutStreamableHandle = NRGameSingleton->StreamableManager.RequestAsyncLoad(TargetsToStream, DelegateToCall, Priority, bManageActiveHandle, bStartStalled, DebugName);
	}
}

void UNRStatics::SetFPS_SeparateFOV(UMeshComponent* Mesh, bool bEnable, bool bSeparate)
{
	if (Mesh)
	{
		const float SeparateFOVAlpha = bEnable ? 1.0f : 0.0f;
		const float SeparateAlpha = bSeparate ? 0.1f : 1.0f;
		Mesh->SetScalarParameterValueOnMaterials(NAME_Separate_FOV_Alpha, SeparateFOVAlpha);
		Mesh->SetScalarParameterValueOnMaterials(NAME_Separate_Alpha, SeparateAlpha);
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

FNRWeaponInformationRow* UNRStatics::GetWeaponInformationRow(ENRWeaponType WeaponType)
{
	if (UNRGameSingleton* NRGameSingleton = UNRGameSingleton::Get())
	{
		if (NRGameSingleton->WeaponType2InfoRowName.Contains(WeaponType))
		{
			const FName& RowName = NRGameSingleton->WeaponType2InfoRowName[WeaponType];
			return NRGameSingleton->WeaponInformationDataTable->FindRow<FNRWeaponInformationRow>(RowName, RowName.ToString());
		}
	}

	return nullptr;
}
