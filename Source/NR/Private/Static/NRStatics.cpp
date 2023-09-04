// Fill out your copyright notice in the Description page of Project Settings.


#include "Static/NRStatics.h"

#include "NRGameSingleton.h"
#include "Engine/DataTable.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
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

void UNRStatics::ConeTraceMultiByChannel(const UWorld* World, TArray<FHitResult>& OutHits, const FVector& Start, const FVector& End, float Angle, ECollisionChannel TraceChannel, const FCollisionQueryParams& Params/* FCollisionQueryParams::DefaultQueryParam */, int32 FindActorNum/* =-1 */, bool bDebug/* =false */, float DebugLifeTime/* =3.0f */)
{
	if (!World || Angle<=0.0f || Angle>=90.0f)
	{
		return;
	}
	
	// 圆锥属性
	const FVector& Dir = (End - Start).GetSafeNormal();
	const float H = (End - Start).Size();
	const float TanAngle = FMath::Tan(FMath::DegreesToRadians(Angle));

#ifdef ENABLE_DRAW_DEBUG
	if (bDebug)
	{
		DrawDebugCone(World, Start, Dir, H, FMath::DegreesToRadians(Angle), FMath::DegreesToRadians(Angle), 8, FColor::Cyan, false, DebugLifeTime);
	}
#endif


	TArray<FHitResult> Hits;
	FCollisionShape Shape; Shape.SetSphere(1.0f);
	FVector TraceStart = Start;
	float TracedLength = 0.0f;
	TSet<const AActor*> Actors;

	do
	{
		float L = Shape.Sphere.Radius / TanAngle;
		if ((TracedLength - Shape.Sphere.Radius/2.0f) + L + Shape.Sphere.Radius*2.0f > H)
		{
			L = H - (TracedLength - Shape.Sphere.Radius/2.0f) - Shape.Sphere.Radius;
		}
		FVector TraceEnd = TraceStart + Dir * L;

#ifdef ENABLE_DRAW_DEBUG
		if (bDebug)
		{
			DrawDebugCapsule(World, (TraceStart+TraceEnd)/2.0f, L/2.0f + Shape.Sphere.Radius, Shape.Sphere.Radius, FRotationMatrix::MakeFromYZ(UKismetMathLibrary::GetRightVector(Dir.Rotation()), Dir).ToQuat(), FColor::Orange, false, DebugLifeTime);
		}
#endif
		Hits.Empty();
		World->SweepMultiByChannel(Hits, TraceStart, TraceEnd, FRotator::ZeroRotator.Quaternion(), TraceChannel, Shape, Params);
		for (const FHitResult& Hit : Hits)
		{
#ifdef ENABLE_DRAW_DEBUG
			if (bDebug)
			{
				DrawDebugPoint(World, Hit.Location, 16.0f, Hit.bBlockingHit ? FColor::Red : FColor::Green, false, DebugLifeTime);
			}
#endif
			if (const AActor* HitActor = Hit.GetActor())
			{
				if (!Actors.Find(HitActor))
				{
					Actors.Add(HitActor);
					OutHits.Add(Hit);

					if (FindActorNum > 0 && Actors.Num() >= FindActorNum)
					{
						return;
					}
				}
			}
			else
			{
				OutHits.Add(Hit);
			}
			
			if (Hit.bBlockingHit)
			{
				return;
			}
		}
		
		TraceStart = TraceEnd;
		TracedLength = (TracedLength - Shape.Sphere.Radius/2.0f) + L + Shape.Sphere.Radius;
		Shape.Sphere.Radius *= 2.0f;
	}
	while (TracedLength < H);
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
