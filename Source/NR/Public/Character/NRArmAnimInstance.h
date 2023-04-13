// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceProxy.h"
#include "Table/Weapon/NRAnimSetting.h"
#include "NRArmAnimInstance.generated.h"

USTRUCT(BlueprintType)
struct NR_API FNRArmAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	FNRArmAnimInstanceProxy():
		VelocityAlpha(0.0f),
		VelocityNormalized(FVector::ZeroVector),
		VelocityPlayRate(1.0f),
		bCrouching(false)
	{}
	FNRArmAnimInstanceProxy(UAnimInstance* Instance): FAnimInstanceProxy(Instance),
		VelocityAlpha(0.0f),
		VelocityNormalized(FVector::ZeroVector),
		VelocityPlayRate(1.0f),
		bCrouching(false)
	{}

	//~Begin FAnimInstanceProxy
	virtual void Initialize(UAnimInstance* InAnimInstance) override;
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;
	//~End   FAnimInstanceProxy

//~Begin This Class
	UPROPERTY(Transient, BlueprintReadOnly)
	FNRAnimSettingRow AnimSetting; // 动画设置
	UPROPERTY(Transient, BlueprintReadOnly)
	float VelocityAlpha;           // 1. 与当前最大移动速度比率 (clamp 0 1) 0:idle不混合移动 1:idle混合移动
	UPROPERTY(Transient, BlueprintReadOnly)
	FVector VelocityNormalized;    // 2. 速度归一化后
	UPROPERTY(Transient, BlueprintReadOnly)
	float VelocityPlayRate;        // 3. 与当前最大移动速度比率

	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bCrouching: 1;           // 4. 是否蹲伏
};

/**
 * 
 */
UCLASS()
class NR_API UNRArmAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FNRArmAnimInstanceProxy mProxy;

protected:
	//~Begin UAnimInstance
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override { return &mProxy; }
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override {}
	//~End   UAnimInstance

	//~PreView Only
public:
	UPROPERTY(EditDefaultsOnly)
	FNRAnimSettingRow PreView_AnimSetting;
};
