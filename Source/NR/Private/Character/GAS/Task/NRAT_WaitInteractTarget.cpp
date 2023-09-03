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
					
					FCollisionQueryParams Params(SCENE_QUERY_STAT(UNRAT_WaitInteractTarget), false);
					Params.AddIgnoredActors(ActorsToIgnore);
					Params.bReturnPhysicalMaterial = true;

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
					
					TArray<FHitResult> HitResults; // Interact_ProfileName-> ObjectType:Ability && OverlapAll ObjectType
					UNRStatics::ConeTraceMultiByProfile(GetWorld(), HitResults, ViewLoc, ViewEnd, Angle, NRCollisionProfile::Interact_ProfileName, Params, 1, bDebug, TracePeriod);
					// 是否找到可交互物
					if (HitResults.Num() > 0)
					{
						if (HitResults[0].Component.IsValid() && HitResults[0].Component.Get()->GetCollisionResponseToChannel(NRCollisionChannel::ECC_Interact) == ECR_Overlap)
						{
							if (HitResults[0].GetActor() && HitResults[0].GetActor()->Implements<UNRInteractInterface>())
							{
								if (!HitResult.GetActor() || HitResults[0].Distance < HitResult.Distance)
								{
									// TODO 距离检查
									HitResult = HitResults[0];
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
