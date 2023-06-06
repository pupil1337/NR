// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceProxy.h"
#include "Engine/StreamableManager.h"
#include "Table/Weapon/NRAnimSetting.h"
#include "NRBodyAnimInstance.generated.h"

USTRUCT(BlueprintType)
struct NR_API FNRMoveDirAlpha
{
	GENERATED_BODY()
	FNRMoveDirAlpha(): Move_F(0.0f), Move_B(0.0f), Move_L(0.0f), Move_R(0.0f) {}
	
	// [0, 1]
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Move_F;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Move_B;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Move_L;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 Forward: 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 Backward: 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 Left: 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 Right: 1;

private:
	void Clear() { Forward = Backward = Left = Right = false; }
};

USTRUCT(BlueprintType)
struct NR_API FTurnDir
{
	GENERATED_BODY()
	FTurnDir(): Left(false), Right(false) {}

	void SetTurnNone() { Clear(); }
	void SetTurnL() { Clear(); Left = true; }
	void SetTurnR() { Clear(); Right = true; }
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 Left: 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 Right: 1;

private:
	void Clear() { Left = Right = false; }
};

USTRUCT(BlueprintType)
struct NR_API FNRAnimCurves
{
	GENERATED_BODY()
	FNRAnimCurves(): bFeetCrossing(false) {}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 bFeetCrossing: 1;
};

class ANRWeaponBase;

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
		bSkiing(false),
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
		bSkiing(false),
		AO_Yaw(0.0f),
		AO_Pitch(0.0f),
		AO_Pitch_Negate(0.0f)
	{}

	virtual ~FNRBodyAnimInstanceProxy() override;

	//~Begin FAnimInstanceProxy
	virtual void Initialize(UAnimInstance* InAnimInstance) override;
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;
	//~End FAnimInstanceProxy

//~ Begin This Class
	void LoadAsset(const ANRWeaponBase* WeaponEquipped);
	void CalculateMoveDirAndAlpha(const FVector& V, float MoveAngle, float DeltaSeconds);
	void UpdateAimOffset(const FRotator& BaseAimRotation, bool bLocallyControlled, float DeltaSeconds);
	float GetCurrMoveModeMaxSpeed() const;
	void UpdateCurvesValue(const UAnimInstance* InAnimInstance);
	void UpdateOtherValues();
	
	// Temp
	float MaxWalkSpeed = 0.0f;
	float MaxCrouchSpeed = 0.0f;
	float MaxRunSpeed = 0.0f;

	/** AnimSetting */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FNRBodyAnimSetRow AnimSetting;

	/** 动画缓存 */
	TPair<const ANRWeaponBase*, TSharedPtr<FStreamableHandle>> StreamableHandlePair;
	
	/** 1. 四个方向输入的值 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FNRMoveDirAlpha MoveDirAlpha;
	
	/** 2. 移动方向 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FNRMoveDir MoveDir;
	
	/** 3. 动画播放速率 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	float VelocityPlayRate;
	
	/** 4. 是否在移动 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bMoving: 1;
	
	/** 5. 是否在跳跃 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bJumping: 1;
	
	/** 6. 是否在下蹲 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bCrouching: 1;
	
	/** 7. 是否在奔跑 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bRunning: 1;
	
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bCrouchingAndMoving: 1;

	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bNotCrouchingAndMoving: 1;

	/** 8. 是否在滑铲 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bSkiing: 1;
	
	/** 9. 瞄准偏移 Yaw */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	float AO_Yaw;
	
	/** 10. 瞄准偏移 Pitch */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	float AO_Pitch;

	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	float AO_Pitch_Negate;

	/** 11. 转身方向 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FTurnDir TurnDir;

	/** 曲线值 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FNRAnimCurves Curves;

	// 临时值
	FRotator StartAimRotation = FRotator::ZeroRotator;
	float InterpAO_Yaw = 0.0f;
};

/**
 * 
 */
UCLASS(Abstract)
class NR_API UNRBodyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FNRBodyAnimInstanceProxy mProxy;

protected:
	//~Begin UAnimInstance
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override { return &mProxy; }
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override {}
	//~End   UAnimInstance
};
