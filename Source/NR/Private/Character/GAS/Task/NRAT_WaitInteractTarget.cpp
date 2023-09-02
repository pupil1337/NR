// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Task/NRAT_WaitInteractTarget.h"

#include "Character/NRCharacter.h"
#include "Interface/NRInteractInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerController/NRPlayerController.h"
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

					FVector ViewLoc;
					FRotator ViewRot;
					PC->GetPlayerViewPoint(ViewLoc, ViewRot);
					FVector ViewEnd = ViewLoc + ViewRot.Vector()*10000.0f;

					FHitResult HitResult;
					HitResult.TraceStart = ViewLoc;
					HitResult.TraceEnd = ViewEnd;
					
					TArray<FHitResult> HitResults; // Interact_ProfileName-> ObjectType:Ability && OverlapAll ObjectType
					
					// 0. Center
					GetWorld()->LineTraceMultiByProfile(HitResults, ViewLoc, ViewEnd, NRCollisionProfile::Interact_ProfileName, Params);
					UpdateHitResult(HitResult, HitResults);

					// 模拟圆锥射线检测 Begin TODO 随着距离的增加，需要的射线数量也需增加，很容易漏检测。所以需要换成球体扫描检测(随着时间增加球体半径变大进行模拟)
					FVector ViewDir = ViewEnd - ViewLoc;
					FVector DirU = UKismetMathLibrary::GetUpVector(ViewRot);
					FVector DirR = UKismetMathLibrary::GetRightVector(ViewRot);
					
					// 1. LU
					FVector EndLU = ViewLoc + ViewDir.RotateAngleAxis(-1.0f, DirU).RotateAngleAxis(-1.0f, DirR);
					HitResults.Empty();
					GetWorld()->LineTraceMultiByProfile(HitResults, ViewLoc, EndLU, NRCollisionProfile::Interact_ProfileName, Params);
					UpdateHitResult(HitResult, HitResults);

					// 2. LD
					FVector EndLD = ViewLoc + ViewDir.RotateAngleAxis(-1.0f, DirU).RotateAngleAxis(1.0f, DirR);
					HitResults.Empty();
					GetWorld()->LineTraceMultiByProfile(HitResults, ViewLoc, EndLD, NRCollisionProfile::Interact_ProfileName, Params);
					UpdateHitResult(HitResult, HitResults);

					// 3. RU
					FVector EndRU = ViewLoc + ViewDir.RotateAngleAxis(1.0f, DirU).RotateAngleAxis(-1.0f, DirR);
					HitResults.Empty();
					GetWorld()->LineTraceMultiByProfile(HitResults, ViewLoc, EndRU, NRCollisionProfile::Interact_ProfileName, Params);
					UpdateHitResult(HitResult, HitResults);

					// 4. RD
					FVector EndRD = ViewLoc + ViewDir.RotateAngleAxis(1.0f, DirU).RotateAngleAxis(1.0f, DirR);
					HitResults.Empty();
					GetWorld()->LineTraceMultiByProfile(HitResults, ViewLoc, EndRD, NRCollisionProfile::Interact_ProfileName, Params);
					UpdateHitResult(HitResult, HitResults);
					// 模拟圆锥射线检测 End
					
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

#ifdef IMGUI_API
					if (PC->DebugConsole.IsValid())
					{
						if (PC->DebugConsole.Get()->InteractTraceDebug)
						{
							FVector DebugTraceStart = HitResult.TraceStart - DirR - DirU + ViewRot.Vector()*10.0f; // 向左下移动一点目的是能看见debug线

							// 0. Center
							DrawDebugLine(GetWorld(), DebugTraceStart, ViewEnd, FColor::Green, false, TracePeriod);
							// 1. LU
							DrawDebugLine(GetWorld(), DebugTraceStart, EndLU, FColor::Green, false, TracePeriod);
							// 2. LD
							DrawDebugLine(GetWorld(), DebugTraceStart, EndLD, FColor::Green, false, TracePeriod);
							// 3. RU
							DrawDebugLine(GetWorld(), DebugTraceStart, EndRU, FColor::Green, false, TracePeriod);
							// 4. RD
							DrawDebugLine(GetWorld(), DebugTraceStart, EndRD, FColor::Green, false, TracePeriod);

							DrawDebugSphere(GetWorld(), ViewEnd, (EndLU-ViewEnd).Size(), 16, FColor::Green, false, TracePeriod);
							
							if (HitResult.GetActor())
							{
								DrawDebugPoint(GetWorld(), HitResult.Location, 16.0f, FColor::Yellow, false, TracePeriod);	
							}
						}
					}
#endif
				}
			}
		}
	}
}


void UNRAT_WaitInteractTarget::UpdateHitResult(OUT FHitResult& HitResult, const TArray<FHitResult>& HitResults) const
{
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
}

FGameplayAbilityTargetDataHandle UNRAT_WaitInteractTarget::MakeTargetData(const FHitResult& HitResult) const
{
	return FGameplayAbilityTargetingLocationInfo().MakeTargetDataHandleFromHitResult(Ability, HitResult);
}

FString UNRAT_WaitInteractTarget::GetDebugString() const
{
	return FString::Printf(TEXT("WaitInteractTarget. Period:%.2fs, CurrentInteraction:%s"), TracePeriod, TargetData.Get(0)->GetHitResult()->GetActor() ? *TargetData.Get(0)->GetHitResult()->GetActor()->GetName() : TEXT("None"));
}
