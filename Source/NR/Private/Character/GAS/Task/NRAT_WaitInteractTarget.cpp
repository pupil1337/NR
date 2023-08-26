// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Task/NRAT_WaitInteractTarget.h"

#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "Character/NRCharacter.h"
#include "Interface/NRInteractInterface.h"
#include "Types/NRCollisionTypes.h"
#include "Types/NRWeaponTypes.h"

UNRAT_WaitInteractTarget::UNRAT_WaitInteractTarget()
{
	// UAbilityTask
	bTickingTask = true;
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
		if (const APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get())
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
					
					FVector ViewStart;
					FRotator ViewRot;
					PC->GetPlayerViewPoint(ViewStart, ViewRot);
					
					TArray<FHitResult> HitResults; // Profile-> ObjectType:Ability && OverlapAll ObjectType
					GetWorld()->LineTraceMultiByProfile(HitResults, ViewStart, ViewStart + ViewRot.Vector()*10000.0f, NRCollisionProfile::Interact_ProfileName, Params);

					// 是否找到可交互物
					bool bFindInteraction = false;
					if (HitResults.Num() > 0)
					{
						if (HitResults[0].Component.IsValid() && HitResults[0].Component.Get()->GetCollisionResponseToChannel(NRCollisionChannel::ECC_Interact) == ECR_Overlap)
						{
							if (const AActor* HitActor = HitResults[0].GetActor())
							{
								if (HitActor->Implements<UNRInteractInterface>())
								{
									// TODO 距离检查
									bFindInteraction = true;
								}
							}
						}	
					}

					if (bFindInteraction)
					{
						GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Green, FString::Printf(TEXT("Find Interaction %s"), *HitResults[0].GetActor()->GetName()));
					}
					else
					{
						GEngine->AddOnScreenDebugMessage(0, 2.0f, FColor::Orange, TEXT("Not Find Interaction"));
					}
				}
			}
		}
	}
}

FString UNRAT_WaitInteractTarget::GetDebugString() const
{
	return FString::Printf(TEXT("WaitInteractTarget. Period:%.2fs, CurrentInteraction:%s"), TracePeriod, "None");
}
