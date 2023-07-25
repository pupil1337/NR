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
	// Sets default values for this actor's properties
	ANRGATA_Trace();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

//~Begin This Class
	void StopTargeting();

private:
	UPROPERTY()
	float MaxRange;
};
