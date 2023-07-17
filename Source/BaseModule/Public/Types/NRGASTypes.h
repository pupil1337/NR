// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ENRAbilityInputID : uint8
{
	EAIID_None = 0		UMETA(DisplayName="None"),
	EAIID_Confirm = 1	UMETA(DisplayName="确认"),
	EAIID_Cancel = 2	UMETA(DisplayName="取消"),
	EAIID_Jump = 3		UMETA(DisplayName="跳跃"),
	EAIID_Crouch = 4	UMETA(DisplayName="蹲伏"),
	EAIID_Run = 5		UMETA(DisplayName="奔跑"),
	EAIID_Ski = 6		UMETA(DisplayName="滑铲")
};
