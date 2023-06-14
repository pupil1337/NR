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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="静止")
	TSoftObjectPtr<UAnimSequenceBase> IdlePose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="静止呼吸")
	TSoftObjectPtr<UAnimSequenceBase> IdleBreath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="瞄准")
	TSoftObjectPtr<UAnimSequenceBase> AimPose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="瞄准呼吸")
	TSoftObjectPtr<UAnimSequenceBase> AimBreath;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="瞄准向前移动")
	TSoftObjectPtr<UAnimSequenceBase> AimWalkF;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="奔跑")
	TSoftObjectPtr<UAnimSequenceBase> RunPose;

	/** 跳跃动画曲线 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="曲线", DisplayName="跳跃Offset Location ")
	TSoftObjectPtr<UCurveVector> JumpOffsetCurveLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="曲线", DisplayName="跳跃Offset Rotation ")
	TSoftObjectPtr<UCurveVector> JumpOffsetCurveRotation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="曲线", DisplayName="着陆Offset Location ")
	TSoftObjectPtr<UCurveVector> LandOffsetCurveLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="曲线", DisplayName="着陆Offset Rotation ")
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="站立-静止")
	TSoftObjectPtr<UAnimSequenceBase> StandIdlePose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="站立-瞄准")
	TSoftObjectPtr<UAnimSequenceBase> StandAimPose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="蹲伏-静止")
	TSoftObjectPtr<UAnimSequenceBase> CrouchIdlePose;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="蹲伏-瞄准")
	TSoftObjectPtr<UAnimSequenceBase> CrouchAimPose;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="全身-奔跑")
	TSoftObjectPtr<UAnimSequenceBase> RunPose;
};


/**
 * 手臂 动画Montage
 */
USTRUCT(BlueprintType)
struct NR_API FNRArmMontageRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="开火")
	TSoftObjectPtr<UAnimMontage> Fire;
};