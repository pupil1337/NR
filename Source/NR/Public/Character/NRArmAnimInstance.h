// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceProxy.h"
#include "Components/TimelineComponent.h"
#include "Table/Weapon/NRAnimSetting.h"
#include "NRArmAnimInstance.generated.h"

USTRUCT(BlueprintType)
struct NR_API FNRArmAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()
	
	FNRArmAnimInstanceProxy():
		bCrouching(false),
		bJumping(false),
		bRunning(false),
		VelocityAlpha(0.0f),
		VelocityNormalized(FVector::ZeroVector),
		VelocityPlayRate(1.0f),
		JumpOffset_Location(FVector::ZeroVector),
		JumpOffset_Rotation(FRotator::ZeroRotator)
	{}
	FNRArmAnimInstanceProxy(UAnimInstance* Instance): FAnimInstanceProxy(Instance),
		bCrouching(false),
		bJumping(false),
		bRunning(false),
		VelocityAlpha(0.0f),
		VelocityNormalized(FVector::ZeroVector),
		VelocityPlayRate(1.0f),
		JumpOffset_Location(FVector::ZeroVector),
		JumpOffset_Rotation(FRotator::ZeroRotator)
	{}
	
protected:
	//~Begin FAnimInstanceProxy
	virtual void Initialize(UAnimInstance* InAnimInstance) override;
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;
	//~End   FAnimInstanceProxy

//~Begin This Class
	float GetCurrMoveModeMaxSpeed() const;
	// Temp
	float MaxWalkSpeed = 0.0f;
	float MaxCrouchSpeed = 0.0f;
	float MaxRunSpeed = 0.0f;
	// Jump Offset
	uint8 JumpStamp = 0;
	float JumpSeconds = 0.0f;
	bool bPlayJump = false;
	uint8 LandStamp = 0;
	float LandSeconds = 0.0f;
	bool bPlayLand = false;
	
	UPROPERTY(Transient, BlueprintReadOnly)
	FNRAnimSettingRow AnimSetting; // 动画设置
	
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bCrouching: 1;           // 1.  是否蹲伏
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bJumping: 1;             // 2.  是否跳跃
	UPROPERTY(Transient, BlueprintReadOnly)
	uint8 bRunning: 1;             // 3.  是否奔跑
	
	UPROPERTY(Transient, BlueprintReadOnly)
	float VelocityAlpha;           // 4.  与当前最大移动速度比率 (clamp 0 1) 0:idle不混合移动 1:idle混合移动
	UPROPERTY(Transient, BlueprintReadOnly)
	FVector VelocityNormalized;    // 5.  速度归一化后
	UPROPERTY(Transient, BlueprintReadOnly)
	float VelocityPlayRate;        // 6.  与当前最大移动速度比率

	UPROPERTY(Transient, BlueprintReadOnly)
	FVector JumpOffset_Location;   // 7.  跳跃时应用曲线值来修改ik_hand_gun位置
	UPROPERTY(Transient, BlueprintReadOnly)
	FRotator JumpOffset_Rotation;  // 8.  跳跃时应用曲线值来修改ik_hand_gun旋转
	UPROPERTY(Transient, BlueprintReadOnly)
	FVector LandOffset_Location;   // 9.  着陆时应用曲线值来修改ik_hand_gun位置
	UPROPERTY(Transient, BlueprintReadOnly)
	FRotator LandOffset_Rotation;  // 10. 着陆时应用曲线值来修改ik_hand_gun旋转
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

	//~PreView Only
#if WITH_EDITORONLY_DATA
public:
	UPROPERTY(EditDefaultsOnly)
	FNRAnimSettingRow PreView_AnimSetting;
#endif
};
