// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceProxy.h"
#include "NRAnimInstanceProxyBase.generated.h"

class ANRWeaponBase;
struct FStreamableHandle;

USTRUCT(BlueprintType)
struct NR_API FNRAnimInstanceProxyBase : public FAnimInstanceProxy
{
	GENERATED_BODY()

	using Super = FAnimInstanceProxy;

	FNRAnimInstanceProxyBase():
		bCrouching(false),
		bJumping(false),
		bRunning(false),
		bSkiing(false),
		VelocityAlpha(0.0f),
		bAiming(false)
	{}

	FNRAnimInstanceProxyBase(UAnimInstance* Instance): FAnimInstanceProxy(Instance),
		bCrouching(false),
		bJumping(false),
		bRunning(false),
		bSkiing(false),
		VelocityAlpha(0.0f),
		bAiming(false)
	{}

	virtual ~FNRAnimInstanceProxyBase() override;

protected:
	//~Begin FAnimInstanceProxy
	virtual void Initialize(UAnimInstance* InAnimInstance) override;
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;
	//~End FAnimInstanceProxy

//~ Begin This Class
	/** 动画缓存 */
	TPair<const ANRWeaponBase*, TSharedPtr<FStreamableHandle>> StreamableHandlePair;
	virtual void AddSoftObjectPathToArray(TArray<FSoftObjectPath>& TargetsToStream) {  }
	void LoadAsset(const ANRWeaponBase* WeaponEquipped, bool bForce = false);
	
	/** 是否蹲伏 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bCrouching: 1;

	/** 是否跳跃 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bJumping: 1;

	/** 是否奔跑 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bRunning: 1;
	
	/** 是否滑铲 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bSkiing: 1;


	/** 与当前最大移动速度比率 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	float VelocityAlpha;

	
	/** 是否瞄准 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	uint8 bAiming: 1;

	// Temp
	float MaxWalkSpeed = 0.0f;
	float MaxCrouchSpeed = 0.0f;
	float MaxRunSpeed = 0.0f;
	float GetCurrMoveModeMaxSpeed() const;
};
