// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NRTA_Trace.generated.h"

UCLASS()
class NR_API ANRTA_Trace : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ANRTA_Trace();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void Tick(float DeltaTime) override;
	
	virtual void ConfirmTargetingAndContinue() override;
	
//~Begin This Class
	void StopTargeting();

	void ConfigParams(const FGameplayAbilityTargetingLocationInfo& InStartLocation, float InMaxRange, FCollisionProfileName InTraceProfile, bool bInTraceFromPlayerViewPoint = true, int32 InNumberOfTraces = 1, int32 InMaxHitResultsPerTrace = 1);

protected:
	virtual void DoTrace(OUT TArray<FHitResult>& OutHitResult, const UWorld* World, const FGameplayTargetDataFilterHandle FilterHandle, const FVector& Start, const FVector& End, FName ProfileName, const FCollisionQueryParams Params) PURE_VIRTUAL(ANRGATA_Trace);

#if ENABLE_DRAW_DEBUG
	virtual void ShowDebugTrace(const TArray<FHitResult>& HitResults) PURE_VIRTUAL(ANRGATA_Trace);
#endif
	
private:
	TArray<FHitResult> PerformTrace(AActor* InSourceActor);

	void AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, OUT FVector& OutTraceEnd, bool bIgnorePitch = false) const;

	static FGameplayAbilityTargetDataHandle MakeTargetData(const TArray<FHitResult>& HitResults);
	
protected:
	/** Trace最大距离 */
	float MaxRange;

	/** 碰撞Profile名称 */
	FCollisionProfileName TraceProfile;
	
	/** 每次Trace数量 */
	int32 NumberOfTraces;
	
	/** 每条Trace造成的HitResult数量 */
	int32 MaxHitResultsPerTrace;

	bool bTraceFromPlayerViewPoint;
};
