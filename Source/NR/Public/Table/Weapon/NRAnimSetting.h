// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Curves/CurveVector.h"
#include "NRAnimSetting.generated.h"

/**
 * 手臂 动画Sequence/跳跃动画曲线
 */
USTRUCT(BlueprintType)
struct NR_API FNRArmAnimSetRow : public FTableRowBase
{
	GENERATED_BODY()

	/** 动画SequenceBase */
	UPROPERTY(EditDefaultsOnly, DisplayName="静止")
	TSoftObjectPtr<UAnimSequenceBase> IdlePose;

	UPROPERTY(EditDefaultsOnly, DisplayName="呼吸")
	TSoftObjectPtr<UAnimSequenceBase> BreathingStandPose;

	UPROPERTY(EditDefaultsOnly, DisplayName="奔跑")
	TSoftObjectPtr<UAnimSequenceBase> RunPose;

	/** 跳跃动画曲线 */
	UPROPERTY(EditDefaultsOnly, Category="曲线", DisplayName="跳跃Offset Location ")
	TSoftObjectPtr<UCurveVector> JumpOffsetCurveLocation;
	UPROPERTY(EditDefaultsOnly, Category="曲线", DisplayName="跳跃Offset Rotation ")
	TSoftObjectPtr<UCurveVector> JumpOffsetCurveRotation;
	UPROPERTY(EditDefaultsOnly, Category="曲线", DisplayName="着陆Offset Location ")
	TSoftObjectPtr<UCurveVector> LandOffsetCurveLocation;
	UPROPERTY(EditDefaultsOnly, Category="曲线", DisplayName="着陆Offset Rotation ")
	TSoftObjectPtr<UCurveVector> LandOffsetCurveRotation;
};

/**
 * 全身 动画Sequence
 */
USTRUCT(BlueprintType)
struct NR_API FNRBodyAnimSetRow : public FTableRowBase
{
	GENERATED_BODY()
	
	/** 动画SequenceBase */
	UPROPERTY(EditDefaultsOnly, DisplayName="全身-静止")
	TSoftObjectPtr<UAnimSequenceBase> IdlePose;

	UPROPERTY(EditDefaultsOnly, DisplayName="全身-奔跑")
	TSoftObjectPtr<UAnimSequenceBase> RunPose;
};
