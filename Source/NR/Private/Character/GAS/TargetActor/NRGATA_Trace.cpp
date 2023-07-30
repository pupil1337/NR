// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/TargetActor/NRGATA_Trace.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"


ANRGATA_Trace::ANRGATA_Trace()
{
	// AActor
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork;

	// AGameplayAbilityTargetActor
	bDestroyOnConfirmation = false;

	
	NumberOfTraces = 1;
	MaxHitResultsPerTrace = 1;
}

void ANRGATA_Trace::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability); // OwningAbility = Ability;

	SetActorTickEnabled(true);
	
	SourceActor = Ability->GetCurrentActorInfo()->AvatarActor.Get();
}

void ANRGATA_Trace::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANRGATA_Trace::ConfirmTargetingAndContinue()
{
	check(ShouldProduceTargetData());
	if (SourceActor)
	{
		if (IsConfirmTargetingAllowed())
		{
			TArray<FHitResult> HitResults = PerformTrace(SourceActor);
			const FGameplayAbilityTargetDataHandle TargetDataHandle = MakeTargetData(HitResults);
			TargetDataReadyDelegate.Broadcast(TargetDataHandle);
		}	
	}
}

TArray<FHitResult> ANRGATA_Trace::PerformTrace(AActor* InSourceActor)
{
	TArray<FHitResult> ReturnHitResults;

	// TODO DoTrace
	
	return ReturnHitResults;
}

FGameplayAbilityTargetDataHandle ANRGATA_Trace::MakeTargetData(const TArray<FHitResult>& HitResults)
{
	FGameplayAbilityTargetDataHandle ReturnTargetDataHandle;

	for (uint32 i = 0; i < HitResults.Num(); ++i)
	{
		FGameplayAbilityTargetData_SingleTargetHit* tTargetData = new FGameplayAbilityTargetData_SingleTargetHit();
		tTargetData->HitResult = HitResults[i];
		ReturnTargetDataHandle.Add(tTargetData);
	}

	return ReturnTargetDataHandle;
}

void ANRGATA_Trace::StopTargeting()
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

