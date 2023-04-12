// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRWeaponAnimSetting.generated.h"


USTRUCT(BlueprintType)
struct FNRWeaponAnimSetting
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> Arm_IdleSeq;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimSequenceBase> Arm_BreathingStandSeq;
};

