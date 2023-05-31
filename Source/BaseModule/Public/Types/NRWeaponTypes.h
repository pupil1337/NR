// Fill out your copyright notice in the Description page of Project Settings.

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

