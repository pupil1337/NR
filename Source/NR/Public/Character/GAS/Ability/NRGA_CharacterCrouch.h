// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GAS/NRGameplayAbility.h"
#include "NRGA_CharacterCrouch.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRGA_CharacterCrouch : public UNRGameplayAbility
{
	GENERATED_BODY()

public:
	UNRGA_CharacterCrouch();
};
