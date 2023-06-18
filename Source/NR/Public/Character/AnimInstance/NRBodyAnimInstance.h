// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRAnimInstanceProxyBase.h"
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
struct NR_API FNRBodyAnimInstanceProxy : public FNRAnimInstanceProxyBase
{
	GENERATED_BODY()

	using Super = FNRAnimInstanceProxyBase;
	
	FNRBodyAnimInstanceProxy():
		bMoving(false),
		bCrouchingAndMoving(false),
		bNotCrouchingAndMoving(false),
		AO_Yaw(0.0f),
		AO_Pitch(0.0f),
		AO_Pitch_Negate(0.0f)
	{}
	
	FNRBodyAnimInstanceProxy(UAnimInstance* Instance): Super(Instance),
		bMoving(false),
		bCrouchingAndMoving(false),
		bNotCrouchingAndMoving(false),
		AO_Yaw(0.0f),
		AO_Pitch(0.0f),
		AO_Pitch_Negate(0.0f)
	{}

	virtual ~FNRBodyAnimInstanceProxy() override;

protected:
	//~Begin FAnimInstanceProxy
	virtual void Initialize(UAnimInstance* InAnimInstance) override;
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;
	//~End FAnimInstanceProxy

	//~Begin FNRAnimInstanceProxyBase
	virtual void AddSoftObjectPathToArray(TArray<FSoftObjectPath>& OutTargetsToStream) override;
	//~End   FNRAnimInstanceProxyBase
	
//~ Begin This Class
	void CalculateMoveDirAndAlpha(const FVector& V, float MoveAngle, float DeltaSeconds);
	void UpdateAimOffset(const FRotator& BaseAimRotation, bool bLocallyControlled, float DeltaSeconds);
	void UpdateCurvesValue(const UAnimInstance* InAnimInstance);
	void UpdateOtherValues();

	/** AnimSetting */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FNRBodyAnimSetRow AnimSetting;
	
	/** 四个方向输入的值 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FNRMoveDirAlpha MoveDirAlpha;
	
	/** 移动方向 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FNRMoveDir MoveDir;
	
	/** 是否在移动 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bMoving: 1;

	/** 蹲伏&&移动 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bCrouchingAndMoving: 1;

	/** !蹲伏&&移动 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bNotCrouchingAndMoving: 1;
	
	/** 瞄准偏移 Yaw */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	float AO_Yaw;
	
	/** 瞄准偏移 Pitch */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	float AO_Pitch;

	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	float AO_Pitch_Negate;

	/** 转身方向 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FTurnDir TurnDir;

	/** 曲线值 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FNRAnimCurves Curves;

	// Temp
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
