// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "NRGATA_Trace.generated.h"

UCLASS()
class NR_API ANRGATA_Trace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ANRGATA_Trace();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void ConfirmTargetingAndContinue() override;
	
//~Begin This Class
	void StopTargeting();

protected:
	virtual void DoTrace(OUT FHitResult& OutHitResult, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params) PURE_VIRTUAL(ANRGATA_Trace);

private:
	TArray<FHitResult> PerformTrace(AActor* InSourceActor);
	
	FGameplayAbilityTargetDataHandle MakeTargetData(const TArray<FHitResult>& HitResults);
	
protected:
	/** Trace最大距离 */
	float MaxRange;

	/** 每次Trace数量 */
	uint32 NumberOfTraces;
	
	/** 每条Trace造成的HitResult数量 */
	uint32 MaxHitResultsPerTrace;
};
