// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceProxy.h"
#include "Table/Weapon/NRAnimSetting.h"
#include "NRBodyAnimInstance.generated.h"

USTRUCT(BlueprintType)
struct NR_API FNRMoveDirAlpha
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
struct NR_API FNRMoveDir
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
struct NR_API FTurnDir
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
struct NR_API FNRAnimCurves
{
	GENERATED_BODY()
	FNRAnimCurves(): bFeetCrossing(false) {}

	UPROPERTY(BlueprintReadOnly)
	uint8 bFeetCrossing: 1;
};

USTRUCT(BlueprintType)
struct NR_API FNRBodyAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	FNRBodyAnimInstanceProxy():
		VelocityPlayRate(1.0f),
		bMoving(false),
		bJumping(false),
		bCrouching(false),
		bRunning(false),
		bCrouchingAndMoving(false),
		bNotCrouchingAndMoving(false),
		AO_Yaw(0.0f),
		AO_Pitch(0.0f),
		AO_Pitch_Negate(0.0f)
	{}
	
	FNRBodyAnimInstanceProxy(UAnimInstance* Instance): FAnimInstanceProxy(Instance),
		VelocityPlayRate(1.0f),
		bMoving(false),
		bJumping(false),
		bCrouching(false),
		bRunning(false),
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
	float GetCurrMoveModeMaxSpeed() const;
	void UpdateCurvesValue(const UAnimInstance* InAnimInstance);
	void UpdateOtherValues();
	
	// Temp
	float MaxWalkSpeed = 0.0f;
	float MaxCrouchSpeed = 0.0f;
	float MaxRunSpeed = 0.0f;

	UPROPERTY(Transient, BlueprintReadOnly)
	FNRAnimSettingRow AnimSetting; // AnimSetting
	
	UPROPERTY(Transient, BlueprintReadOnly)
	FNRMoveDirAlpha MoveDirAlpha;  // 1.  四个方向输入的值
	UPROPERTY(Transient, BlueprintReadOnly)
	FNRMoveDir MoveDir;            // 2.  移动方向
	UPROPERTY(Transient, BlueprintReadOnly)
	float VelocityPlayRate;        // 3.  动画播放速率
	
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bMoving: 1;              // 4.  是否在移动
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bJumping: 1;             // 5.  是否在跳跃
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bCrouching: 1;           // 6.  是否在下蹲
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bRunning: 1;             // 7.  是否在奔跑
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bCrouchingAndMoving: 1;
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bNotCrouchingAndMoving: 1;

	UPROPERTY(Transient, BlueprintReadOnly)
	float AO_Yaw;                  // 8.  瞄准偏移 Yaw
	UPROPERTY(Transient, BlueprintReadOnly)
	float AO_Pitch;                // 9.  瞄准偏移 Pitch
	UPROPERTY(Transient, BlueprintReadOnly)
	float AO_Pitch_Negate;
	UPROPERTY(Transient, BlueprintReadOnly)
	FTurnDir TurnDir;              // 10. 转身方向

	UPROPERTY(Transient, BlueprintReadOnly)
	FNRAnimCurves Curves;          // 曲线值

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

	//~PreView Only
#if WITH_EDITORONLY_DATA
public:
	UPROPERTY(EditDefaultsOnly)
	FNRAnimSettingRow PreView_AnimSetting;
#endif
};
