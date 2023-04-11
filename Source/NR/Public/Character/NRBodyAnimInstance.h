// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceProxy.h"
#include "NRBodyAnimInstance.generated.h"

USTRUCT(BlueprintType)
struct FNRMoveDirAlpha
{
	GENERATED_BODY()
	FNRMoveDirAlpha(): Move_F(0.0f), Move_B(0.0f), Move_L(0.0f), Move_R(0.0f) {}
	
	// [0, 1]
	UPROPERTY(BlueprintReadOnly)
	float Move_F;
	UPROPERTY(BlueprintReadOnly)
	float Move_B;
	UPROPERTY(BlueprintReadOnly)
	float Move_L;
	UPROPERTY(BlueprintReadOnly)
	float Move_R;
};

USTRUCT(BlueprintType)
struct FNRMoveDir
{
	GENERATED_BODY()
	FNRMoveDir(): Forward(false), Backward(false), Left(false), Right(false) {}

	void Clear() { Forward = Backward = Left = Right = false; }
	void SetMoveF() { Clear(); Forward = true; }
	void SetMoveB() { Clear(); Backward = true; }
	void SetMoveL() { Clear(); Left = true; }
	void SetMoveR() { Clear(); Right = true; }
	
	UPROPERTY(BlueprintReadOnly)
	uint8 Forward: 1;
	UPROPERTY(BlueprintReadOnly)
	uint8 Backward: 1;
	UPROPERTY(BlueprintReadOnly)
	uint8 Left: 1;
	UPROPERTY(BlueprintReadOnly)
	uint8 Right: 1;
};

USTRUCT(BlueprintType)
struct FNRAnimCurves
{
	GENERATED_BODY()
	FNRAnimCurves(): bFeetCrossing(false) {}

	UPROPERTY(BlueprintReadOnly)
	uint8 bFeetCrossing: 1;
};

USTRUCT(BlueprintType)
struct FNRBodyAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	FNRBodyAnimInstanceProxy():
		bMoving(false)
	{}
	FNRBodyAnimInstanceProxy(UAnimInstance* Instance): FAnimInstanceProxy(Instance),
		bMoving(false)
	{}

	//~Begin FAnimInstanceProxy
	virtual void Initialize(UAnimInstance* InAnimInstance) override;
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;
	//~End FAnimInstanceProxy

//~ Begin This Class
	void CalculateMoveDirAndAlpha(const FVector& V, float DeltaSeconds);
	void UpdateCurvesValue(UAnimInstance* InAnimInstance);
	
	UPROPERTY(Transient, BlueprintReadOnly)
	FNRMoveDirAlpha MoveDirAlpha; // 四个方向输入的值
	UPROPERTY(Transient, BlueprintReadOnly)
	FNRMoveDir MoveDir; // 移动方向
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bMoving: 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	FNRAnimCurves Curves;
};

/**
 * 
 */
UCLASS()
class NR_API UNRBodyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FNRBodyAnimInstanceProxy mProxy;

protected:
	//~Begin UAnimInstance
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override { return &mProxy; }
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override {}
	//~End   UAnimInstance
};
