// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Task/NRAT_WaitInteractTarget.h"

#include "Character/NRCharacter.h"
#include "Interface/NRInteractInterface.h"
#include "PlayerController/NRPlayerController.h"
#include "Static/NRStatics.h"
#include "Types/NRCollisionTypes.h"

UNRAT_WaitInteractTarget::UNRAT_WaitInteractTarget()
{
	// UAbilityTask
	bTickingTask = true;

	TargetData = MakeTargetData(FHitResult());
}

UNRAT_WaitInteractTarget* UNRAT_WaitInteractTarget::WaitInteractTarget(UGameplayAbility* OwningAbility, float Period/*=0.1f*/)
{
	UNRAT_WaitInteractTarget* MyObj = NewAbilityTask<UNRAT_WaitInteractTarget>(OwningAbility);
	MyObj->TracePeriod = Period;
	
	return MyObj;
}

void UNRAT_WaitInteractTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	if (!GetWorld())
	{
		check(false);
		return;
	}
	
	if (Ability)
	{
		if (const ANRPlayerController* PC = Cast<ANRPlayerController>(Ability->GetCurrentActorInfo()->PlayerController.Get()))
		{
			CurrTime += DeltaTime;
			if (TracePeriod >= 0.0f && CurrTime >= LastTraceTime + TracePeriod)
			{
				LastTraceTime = CurrTime;
				// 每隔Period秒检测一次

				if (AActor* SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get())
				{
					bool bDebug = false;
					float Angle = 2.0f;
					float MaxDistance = 10000.0f;
#ifdef IMGUI_API
					if (PC->DebugConsole.IsValid())
					{
						bDebug = PC->DebugConsole.Get()->InteractTraceDebug;
						Angle = PC->DebugConsole.Get()->InteractAngle;
						MaxDistance = PC->DebugConsole.Get()->InteractDistance;
					}
#endif
					
					FVector ViewLoc;
					FRotator ViewRot;
					PC->GetPlayerViewPoint(ViewLoc, ViewRot);
					FVector ViewEnd = ViewLoc + ViewRot.Vector()*MaxDistance;

					FCollisionQueryParams Params(SCENE_QUERY_STAT(UNRAT_WaitInteractTarget), false);
					Params.AddIgnoredActor(SourceActor);
					
					TArray<FHitResult> HitResults; // Interact Channel: Overlap Interaction, Ignore Others
					UNRStatics::ConeTraceMultiByChannel(GetWorld(), HitResults, ViewLoc, ViewEnd, Angle, NRCollisionChannel::ECC_Interact, Params, bDebug, TracePeriod);

					FHitResult HitResult;
					HitResult.TraceStart = ViewLoc;
					HitResult.TraceEnd = ViewEnd;
					for (const FHitResult& Hit : HitResults)
					{
						if (AActor* HitActor = Hit.GetActor())
						{
							if (HitActor->Implements<UNRInteractInterface>())
							{
								FHitResult CameraHit;
								FCollisionQueryParams ViewParams;
								ViewParams.AddIgnoredActor(SourceActor);
								ViewParams.AddIgnoredActor(HitActor);
								if (GetWorld()->LineTraceSingleByChannel(CameraHit, ViewLoc, Hit.ImpactPoint, ECC_Camera, ViewParams))
								{
#ifdef ENABLE_DRAW_DEBUG
									if (bDebug)
									{
										DrawDebugPoint(GetWorld(), CameraHit.ImpactPoint, 16.0f, FColor::Red, false, TracePeriod);
									}
#endif
								}
								else
								{
									// TODO 距离检查
									HitResult = Hit;
									break;
								}
							}	
						}
					}
					
					const AActor* OldActor = TargetData.Get(0)->GetHitResult()->GetActor();
					const AActor* NewActor = HitResult.GetActor();

					if (NewActor != OldActor)
					{
						TargetData = MakeTargetData(HitResult);
						UpdateTarget.Broadcast(TargetData);
					}
				}
			}
		}
	}
}

FGameplayAbilityTargetDataHandle UNRAT_WaitInteractTarget::MakeTargetData(const FHitResult& HitResult) const
{
	return FGameplayAbilityTargetingLocationInfo().MakeTargetDataHandleFromHitResult(Ability, HitResult);
}

FString UNRAT_WaitInteractTarget::GetDebugString() const
{
	return FString::Printf(TEXT("WaitInteractTarget. Period:%.2fs, CurrentInteraction:%s"), TracePeriod, TargetData.Get(0)->GetHitResult()->GetActor() ? *TargetData.Get(0)->GetHitResult()->GetActor()->GetName() : TEXT("None"));
}
