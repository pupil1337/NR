﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


/** 穿模修复、FOV修复 */
const FName NAME_Separate_FOV_Alpha(TEXT("Separate_FOV Alpha"));
/** 穿模修复 */
const FName NAME_Separate_Alpha(TEXT("Separate Alpha"));

UENUM()
enum class ENRWeaponState : uint8
{
	EWS_None = 0,
	EWS_Pickup = 1,
	EWS_Equip = 2
};

UENUM()
enum class ENRWeaponFireMode : uint8
{
	EWFM_Semi = 0		UMETA(DisplayName="半自动"),
	EWFM_Automatic = 1	UMETA(DisplayName="全自动"),
	EWFM_Burst = 2		UMETA(DisplayName="连发")
};

