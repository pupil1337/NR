// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Task/NRAT_WaitInteractTarget.h"

#include "Abilities/GameplayAbilityTargetActor_Trace.h"
#include "Character/NRCharacter.h"
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

	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(OwningAbility->GetCurrentActorInfo()->AvatarActor.Get()))
	{
		MyObj->StartLocation1P = FGameplayAbilityTargetingLocationInfo();
		MyObj->StartLocation1P.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
		MyObj->StartLocation1P.SourceComponent = NRCharacter->GetMeshArm();
		MyObj->StartLocation1P.SourceSocketName = NAME_Socket_Weapon;

		MyObj->StartLocation3P = FGameplayAbilityTargetingLocationInfo();
		MyObj->StartLocation3P.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
		MyObj->StartLocation3P.SourceComponent = NRCharacter->GetMesh();
		MyObj->StartLocation3P.SourceSocketName = NAME_Socket_Weapon;
	}
	
	return MyObj;
}

void UNRAT_WaitInteractTarget::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	CurrTime += DeltaTime;
	if (TracePeriod >= 0.0f && CurrTime >= LastTraceTime + TracePeriod)
	{
		{ /* -> PerformTrace */
			if (AActor* SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get())
			{
				TArray<AActor*> ActorsToIgnore;
				ActorsToIgnore.Add(SourceActor);

				FCollisionQueryParams Params(SCENE_QUERY_STAT(UNRAT_WaitInteractTarget), false);
				Params.AddIgnoredActors(ActorsToIgnore);
				Params.bReturnPhysicalMaterial = true;

				{ /* -> AimWithPlayerController */
					if (Ability)
					{
						// TraceStart为描述目标开始位置的信息
						const FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();

						FVector ViewStart = TraceStart;
						FRotator ViewRot = FRotator::ZeroRotator;
						if (const APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get())
						{
							PC->GetPlayerViewPoint(ViewStart, ViewRot);
						}
						const FVector ViewDir = ViewRot.Vector();
						FVector TraceEnd = ViewStart + ViewDir * 10000.0f;

						AGameplayAbilityTargetActor_Trace::ClipCameraRayToAbilityRange(ViewStart, ViewDir, TraceStart, 10000.0f, TraceEnd);

						{ /* -> LineTrace */
							TArray<FHitResult> HitResults;
							GetWorld()->LineTraceMultiByProfile(HitResults, TraceStart, TraceEnd, NRCollisionProfile::Interact_ProfileName, Params);

							for (int32 i = 0; i < HitResults.Num(); ++i)
							{
								const FHitResult& Hit = HitResults[i];
								if (Hit.GetActor())
								{
									
								}
							}
						}
					}
				}
			}
		}

		LastTraceTime = CurrTime;
	}
}

FString UNRAT_WaitInteractTarget::GetDebugString() const
{
	return FString::Printf(TEXT("WaitInteractTarget. Period:%.2fs, CurrentInteraction:%s"), TracePeriod, "None");
}
