// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTags/Classes/GameplayTagContainer.h"

UENUM(BlueprintType)
enum class ENRAbilityInputID : uint8
{
	EAIID_None		UMETA(DisplayName="None"),
	EAIID_Confirm	UMETA(DisplayName="确认"),
	EAIID_Cancel	UMETA(DisplayName="取消"),

	// Character Movement
	EAIID_Jump		UMETA(DisplayName="跳跃"),
	EAIID_Crouch	UMETA(DisplayName="蹲伏"),
	EAIID_Run		UMETA(DisplayName="奔跑"),
	EAIID_Ski		UMETA(DisplayName="滑铲"),

	// Weapon Action
	EAIID_Fire		UMETA(DisplayName="开火"),

	// Interact
	EAIID_Interact	UMETA(DisplayName="交互")
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
	static FGameplayTag GA_Interact = FGameplayTag::RequestGameplayTag("Ability.Interact");
}
