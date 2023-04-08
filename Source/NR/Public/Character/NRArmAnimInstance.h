// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceProxy.h"
#include "NRArmAnimInstance.generated.h"

USTRUCT(BlueprintType)
struct FNRArmAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	FNRArmAnimInstanceProxy(): FAnimInstanceProxy(),
		MoveDir(FVector2D::ZeroVector)
	{}
	FNRArmAnimInstanceProxy(UAnimInstance* Instance): FAnimInstanceProxy(Instance) {}

	//~Begin FAnimInstanceProxy
	virtual void Initialize(UAnimInstance* InAnimInstance) override;
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;
	//~End   FAnimInstanceProxy

private:
	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FVector2D MoveDir;
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
};
