// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Curves/CurveVector.h"
#include "NRAnimSetting.generated.h"

USTRUCT(BlueprintType)
struct NR_API FNRArmAnimSequence
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="静止")
	TObjectPtr<UAnimSequenceBase> IdlePose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="呼吸-站立")
	TObjectPtr<UAnimSequenceBase> BreathingStandPose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="奔跑")
	TObjectPtr<UAnimSequenceBase> RunPose;
};

USTRUCT(BlueprintType)
struct NR_API FNRBodyAnimSequence
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, DisplayName="静止")
	TObjectPtr<UAnimSequenceBase> IdlePose;
};

USTRUCT(BlueprintType)
struct NR_API FNRAnimSettingRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="动画", DisplayName="手臂动画")
	FNRArmAnimSequence ArmAnimSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="动画", DisplayName="全身动画")
	FNRBodyAnimSequence BodyAnimSequence;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="曲线", DisplayName="跳跃Offset Location ")
	TObjectPtr<UCurveVector> JumpOffsetCurveLocation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="曲线", DisplayName="跳跃Offset Rotation ")
	TObjectPtr<UCurveVector> JumpOffsetCurveRotation;
};
