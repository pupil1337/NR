// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceProxy.h"
#include "NRBodyAnimInstance.generated.h"

UENUM(BlueprintType)
enum class ENRMoveDir: uint8
{
	EMD_Forward,
	EMD_Backward,
	EMD_Left,
	EMD_Right
};

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
struct FNRBodyAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	FNRBodyAnimInstanceProxy():
		MoveDir(ENRMoveDir::EMD_Forward),
		bMoving(false)
	{}
	FNRBodyAnimInstanceProxy(UAnimInstance* Instance): FAnimInstanceProxy(Instance),
		MoveDir(ENRMoveDir::EMD_Forward),
		bMoving(false)
	{}

	//~Begin FAnimInstanceProxy
	virtual void Initialize(UAnimInstance* InAnimInstance) override;
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;
	//~End FAnimInstanceProxy

//~ Begin This Class
private:
	void CalculateMoveDirAndAlpha(const FVector& V, float DeltaSeconds);
	
	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FNRMoveDirAlpha MoveDirAlpha; // 四个方向输入的值
	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	ENRMoveDir MoveDir; // 移动方向
	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	uint8 bMoving: 1;
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
