// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags/Classes/GameplayTagContainer.h"

UENUM(BlueprintType)
enum class ENRAbilityInputID : uint8
{
	EAIID_None = 0		UMETA(DisplayName="None"),
	EAIID_Confirm = 1	UMETA(DisplayName="确认"),
	EAIID_Cancel = 2	UMETA(DisplayName="取消"),

	// Character Movement
	EAIID_Jump = 3		UMETA(DisplayName="跳跃"),
	EAIID_Crouch = 4	UMETA(DisplayName="蹲伏"),
	EAIID_Run = 5		UMETA(DisplayName="奔跑"),
	EAIID_Ski = 6		UMETA(DisplayName="滑铲"),

	// Weapon Action
	EAIID_Fire = 7		UMETA(DisplayName="开火")
};

namespace NRGameplayTag
{
	// State
	static FGameplayTag State_Falling = FGameplayTag::RequestGameplayTag("State.Falling");
	static FGameplayTag State_RecentlyDamaged = FGameplayTag::RequestGameplayTag("State.RecentlyDamaged");

	// Data
	static FGameplayTag Data_Damage = FGameplayTag::RequestGameplayTag("Data.Damage");

	// GC
	static FGameplayTag GC_Weapon_Fire = FGameplayTag::RequestGameplayTag(FName("GameplayCue.Weapon.Fire"));

	// GA
	static FGameplayTag GA_Run = FGameplayTag::RequestGameplayTag("Ability.Run");
}
