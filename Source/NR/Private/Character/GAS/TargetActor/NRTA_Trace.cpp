// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/TargetActor/NRTA_Trace.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTargetActor_Trace.h"


ANRTA_Trace::ANRTA_Trace()
{
	// AActor
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;

	// AGameplayAbilityTargetActor
	bDestroyOnConfirmation = false;


	MaxRange = 999999.0f;
	TraceProfile = UCollisionProfile::BlockAll_ProfileName;
	NumberOfTraces = 1;
	MaxHitResultsPerTrace = 1;
	bTraceFromPlayerViewPoint = true;
}

void ANRTA_Trace::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability); // OwningAbility = Ability;

	SetActorTickEnabled(true);
	
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void ANRTA_Trace::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANRTA_Trace::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (SourceActor)
	{
		if (IsConfirmTargetingAllowed())
		{
			const TArray<FHitResult> HitResults = PerformTrace(SourceActor);
			const FGameplayAbilityTargetDataHandle TargetDataHandle = MakeTargetData(HitResults);
			TargetDataReadyDelegate.Broadcast(TargetDataHandle);

#if ENABLE_DRAW_DEBUG
			if (bDebug)
			{
				ShowDebugTrace(HitResults, EDrawDebugTrace::Type::ForDuration, 3.0f);
			}
#endif
		}	
	}
}

TArray<FHitResult> ANRTA_Trace::PerformTrace(AActor* InSourceActor)
{
	bool bTraceComplex = false;
	TArray<AActor*> ActorsToIgnore;
	
	ActorsToIgnore.Add(InSourceActor);

	FCollisionQueryParams Params(SCENE_QUERY_STAT(ANRTA_Trace), bTraceComplex);
	Params.AddIgnoredActors(ActorsToIgnore);
	Params.bReturnPhysicalMaterial = true;

	// TraceStart为SourceActor的位置 非摄像机位置
	FVector TraceStart = StartLocation.GetTargetingTransform().GetLocation();// InSourceActor->GetActorLocation();
	FVector TraceEnd;

	// 如果从摄像机位置Trace, 则TraceStart更改为摄像机位置
	if (PrimaryPC && bTraceFromPlayerViewPoint)
	{
		FVector ViewStart;
		FRotator ViewRot;
		PrimaryPC->GetPlayerViewPoint(ViewStart, ViewRot);

		TraceStart = ViewStart;
	}

	AimWithPlayerController(InSourceActor, Params, TraceStart, TraceEnd);
	
	TArray<FHitResult> ReturnHitResults;
	for (int32 TraceIdx = 0; TraceIdx < NumberOfTraces; ++TraceIdx)
	{
		TArray<FHitResult> HitResults;
		DoTrace(HitResults, InSourceActor->GetWorld(), Filter, TraceStart, TraceEnd, TraceProfile.Name, Params);

		for (int32 HitResRIdx = HitResults.Num() - 1; HitResRIdx >= 0; --HitResRIdx)
		{
			if (MaxHitResultsPerTrace >= 0 && HitResRIdx + 1 > MaxHitResultsPerTrace)
			{
				HitResults.RemoveAt(HitResRIdx);
			}
		}

		if (HitResults.Num() < 1)
		{
			FHitResult HitResult;
			HitResult.TraceStart = TraceStart;
			HitResult.TraceEnd = TraceEnd;
			HitResult.Location = TraceEnd;
			HitResult.ImpactPoint = TraceEnd;
			HitResults.Add(HitResult);
		}

		ReturnHitResults.Append(HitResults);
	}

	return ReturnHitResults;
}

void ANRTA_Trace::AimWithPlayerController(const AActor* InSourceActor, FCollisionQueryParams Params, const FVector& TraceStart, OUT FVector& OutTraceEnd, bool bIgnorePitch) const
{
	if (!OwningAbility) // Server and launching client only
	{
		return;
	}

	APlayerController* PC = OwningAbility->GetCurrentActorInfo()->PlayerController.Get();
	check(PC);

	FVector ViewStart;
	FRotator ViewRot;
	PC->GetPlayerViewPoint(ViewStart, ViewRot);

	const FVector ViewDir = ViewRot.Vector();
	FVector ViewEnd = ViewStart + (ViewDir * MaxRange);

	AGameplayAbilityTargetActor_Trace::ClipCameraRayToAbilityRange(ViewStart, ViewDir, TraceStart, MaxRange, ViewEnd);

	OutTraceEnd = ViewEnd;
}

FGameplayAbilityTargetDataHandle ANRTA_Trace::MakeTargetData(const TArray<FHitResult>& HitResults)
{
	FGameplayAbilityTargetDataHandle ReturnTargetDataHandle;

	for (int32 i = 0; i < HitResults.Num(); ++i)
	{
		FGameplayAbilityTargetData_SingleTargetHit* tTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
		tTargetData->HitResult = HitResults[i];
		ReturnTargetDataHandle.Add(tTargetData);
	}

	return ReturnTargetDataHandle;
}

void ANRTA_Trace::StopTargeting()
{
	SetActorTickEnabled(false);
	
	// Clear added callbacks
	TargetDataReadyDelegate.Clear();
	CanceledDelegate.Clear();

	// Copy From AGameplayAbilityTargetActor::EndPlay
	if (GenericDelegateBoundASC)
	{
		// We must remove ourselves from GenericLocalConfirmCallbacks/GenericLocalCancelCallbacks, since while these are bound they will inhibit any *other* abilities
		// that are bound to the same key.

		UAbilitySystemComponent* UnboundASC = nullptr;
		const FGameplayAbilityActorInfo* Info = (OwningAbility ? OwningAbility->GetCurrentActorInfo() : nullptr);
		if (Info && Info->IsLocallyControlled())
		{
			UAbilitySystemComponent* ASC = Info->AbilitySystemComponent.Get();
			if (ASC)
			{
				ASC->GenericLocalConfirmCallbacks.RemoveDynamic(this, &AGameplayAbilityTargetActor::ConfirmTargeting);
				ASC->GenericLocalCancelCallbacks.RemoveDynamic(this, &AGameplayAbilityTargetActor::CancelTargeting);

				UnboundASC = ASC;
			}
		}

		ensure(GenericDelegateBoundASC == UnboundASC); // Error checking that we have removed delegates from the same ASC we bound them to
	}
}

void ANRTA_Trace::ConfigParams(float InMaxRange, FCollisionProfileName InTraceProfile, bool bInTraceFromPlayerViewPoint, int32 InNumberOfTraces, int32 InMaxHitResultsPerTrace)
{
	MaxRange = InMaxRange;
	TraceProfile = InTraceProfile;
	bTraceFromPlayerViewPoint = bInTraceFromPlayerViewPoint;
	NumberOfTraces = InNumberOfTraces;
	MaxHitResultsPerTrace = InMaxHitResultsPerTrace;
}

