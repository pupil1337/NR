// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ENRAbilityInputID : uint8
{
	IA_None = 0,
	IA_Confirm = 1,
	IA_Cancel = 2,
	IA_Fire = 3
};
