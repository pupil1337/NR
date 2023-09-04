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
					TArray<AActor*> ActorsToIgnore;
					ActorsToIgnore.Add(SourceActor);

					bool bDebug = false;
					float Angle = 2;
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

					FHitResult HitResult;
					HitResult.TraceStart = ViewLoc;
					HitResult.TraceEnd = ViewEnd;

					FCollisionQueryParams Params(SCENE_QUERY_STAT(UNRAT_WaitInteractTarget), false);
					Params.AddIgnoredActors(ActorsToIgnore);
					
					TArray<FHitResult> HitResults; // Interact Channel: Overlap Interaction, Ignore Others
					UNRStatics::ConeTraceMultiByChannel(GetWorld(), HitResults, ViewLoc, ViewEnd, Angle, NRCollisionChannel::ECC_Interact, Params, 0, bDebug, TracePeriod);

					for (const FHitResult& Hit : HitResults)
					{
						if (AActor* HitActor = Hit.GetActor())
						{
							if (HitActor->Implements<UNRInteractInterface>())
							{
								// TODO 可视性检查 (重新执行一次ConeTrace, Channel改为Camera)
								FHitResult ViewHit;
								FCollisionQueryParams ViewParams;
								ViewParams.AddIgnoredActor(SourceActor);
								ViewParams.AddIgnoredActor(HitActor);
								if (GetWorld()->LineTraceSingleByChannel(ViewHit, ViewLoc, HitActor->GetActorLocation(), ECC_Camera, ViewParams))
								{
#ifdef ENABLE_DRAW_DEBUG
									if (bDebug)
									{
										DrawDebugPoint(GetWorld(), ViewHit.Location, 16.0f, FColor::Red, false, TracePeriod);
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
					
					const AActor* OldTarget = TargetData.Get(0)->GetHitResult()->GetActor();
					const AActor* NewTarget = HitResult.GetActor();

					// Broadcast OldTarget
					if (OldTarget && OldTarget != NewTarget)
					{
						LoseTarget.Broadcast(TargetData);
					}
					
					// Update TargetData
					TargetData = MakeTargetData(HitResult);

					// Broadcast NewTarget
					if (NewTarget && NewTarget != OldTarget)
					{
						FindTarget.Broadcast(TargetData);
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
