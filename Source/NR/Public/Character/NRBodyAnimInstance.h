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

private:
	void Clear() { Forward = Backward = Left = Right = false; }
};

USTRUCT(BlueprintType)
struct FTurnDir
{
	GENERATED_BODY()
	FTurnDir(): None(true), Left(false), Right(false) {}
	
	void SetTurnNone() { Clear(); None = true; }
	void SetTurnL() { Clear(); Left = true; }
	void SetTurnR() { Clear(); Right = true; }
	
	UPROPERTY(BlueprintReadOnly)
	uint8 None: 1;
	UPROPERTY(BlueprintReadOnly)
	uint8 Left: 1;
	UPROPERTY(BlueprintReadOnly)
	uint8 Right: 1;

private:
	void Clear() { None = Left = Right = false; }
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
		bMoving(false),
		bJumping(false),
		bCrouching(false),
		bCrouchingAndMoving(false),
		bNotCrouchingAndMoving(false),
		AO_Yaw(0.0f),
		AO_Pitch(0.0f),
		AO_Pitch_Negate(0.0f)
	{}
	
	FNRBodyAnimInstanceProxy(UAnimInstance* Instance): FAnimInstanceProxy(Instance),
		bMoving(false),
		bJumping(false),
		bCrouching(false),
		bCrouchingAndMoving(false),
		bNotCrouchingAndMoving(false),
		AO_Yaw(0.0f),
		AO_Pitch(0.0f),
		AO_Pitch_Negate(0.0f)
	{}

	//~Begin FAnimInstanceProxy
	virtual void Initialize(UAnimInstance* InAnimInstance) override;
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;
	//~End FAnimInstanceProxy

//~ Begin This Class
	void CalculateMoveDirAndAlpha(const FVector& V, float MoveAngle, float DeltaSeconds);
	void UpdateAimOffset(const FRotator& BaseAimRotation, bool bLocallyControlled, float DeltaSeconds);
	void UpdateCurvesValue(UAnimInstance* InAnimInstance);
	void UpdateOtherValues();
	
	UPROPERTY(Transient, BlueprintReadOnly)
	FNRMoveDirAlpha MoveDirAlpha; // 1. 四个方向输入的值
	UPROPERTY(Transient, BlueprintReadOnly)
	FNRMoveDir MoveDir;           // 2. 移动方向
	
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bMoving: 1;             // 3. 是否在移动
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bJumping: 1;            // 4. 是否在跳跃
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bCrouching: 1;          // 5. 是否在下蹲
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bCrouchingAndMoving: 1;
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bNotCrouchingAndMoving: 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	float AO_Yaw;                 // 6. 瞄准偏移 Yaw
	UPROPERTY(Transient, BlueprintReadOnly)
	float AO_Pitch;               // 7. 瞄准偏移 Pitch
	UPROPERTY(Transient, BlueprintReadOnly)
	float AO_Pitch_Negate;
	UPROPERTY(Transient, BlueprintReadOnly)
	FTurnDir TurnDir;             // 8. 转身方向

	UPROPERTY(Transient, BlueprintReadOnly)
	FNRAnimCurves Curves;         // 曲线值

	// 临时值
	FRotator StartAimRotation = FRotator::ZeroRotator;
	float InterpAO_Yaw = 0.0f;
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
