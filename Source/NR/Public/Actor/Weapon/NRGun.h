// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRWeaponBase.h"
#include "NRGun.generated.h"

UCLASS(Abstract, Blueprintable)
class NR_API ANRGun : public ANRWeaponBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANRGun();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
