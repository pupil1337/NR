// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRAnimInstanceProxyBase.h"
#include "Table/Weapon/NRAnimSetting.h"
#include "NRArmAnimInstance.generated.h"

class ANRWeaponBase;

USTRUCT(BlueprintType)
struct NR_API FNRArmAnimInstanceProxy : public FNRAnimInstanceProxyBase
{
	GENERATED_BODY()

	using Super = FNRAnimInstanceProxyBase;
	
	FNRArmAnimInstanceProxy():
		VelocityNormalized(FVector::ZeroVector),
		JumpOffset_Location(FVector::ZeroVector),
		JumpOffset_Rotation(FRotator::ZeroRotator)
	{}
	
	FNRArmAnimInstanceProxy(UAnimInstance* Instance): Super(Instance),
		VelocityNormalized(FVector::ZeroVector),
		JumpOffset_Location(FVector::ZeroVector),
		JumpOffset_Rotation(FRotator::ZeroRotator)
	{}
	
protected:
	//~Begin FAnimInstanceProxy
	virtual void Initialize(UAnimInstance* InAnimInstance) override;
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;
	//~End   FAnimInstanceProxy

	//~Begin FNRAnimInstanceProxyBase
	virtual void AddSoftObjectPathToArray(TArray<FSoftObjectPath>& AssetsToLoad) override;
	//~End   FNRAnimInstanceProxyBase
	
//~Begin This Class
	/** 动画设置 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FNRArmAnimSetRow AnimSetting;

	/** 速度归一化后 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FVector VelocityNormalized;

	/** 跳跃时应用曲线值来修改ik_hand_gun位置 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FVector JumpOffset_Location;

	/** 跳跃时应用曲线值来修改ik_hand_gun旋转 */
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly)
	FRotator JumpOffset_Rotation;

	// Temp
	// Jump Offset
	uint8 JumpStamp = 0;
	float JumpSeconds = 0.0f;
	bool bPlayJump = false;
	uint8 LandStamp = 0;
	float LandSeconds = 0.0f;
	bool bPlayLand = false;
};

/**
 * 
 */
UCLASS(Abstract)
class NR_API UNRArmAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	UPROPERTY(Transient, EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FNRArmAnimInstanceProxy mProxy;

protected:
	//~Begin UAnimInstance
	virtual void NativeInitializeAnimation() override;
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override { return &mProxy; }
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override {}
	//~End   UAnimInstance

//~Begin This Class
public:
	uint8 JumpStamp = 0;
	uint8 LandStamp = 0;
	UFUNCTION()
	void OnJumped();
	UFUNCTION()
	void OnLanded(const FHitResult& Hit);
};
