// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstanceProxy.h"
#include "NRBodyAnimInstance.generated.h"

USTRUCT(BlueprintType)
struct FNRBodyAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	FNRBodyAnimInstanceProxy(): FAnimInstanceProxy(),
		MoveDir(FVector2D::ZeroVector)
	{}
	FNRBodyAnimInstanceProxy(UAnimInstance* Instance): FAnimInstanceProxy(Instance) {}

	//~Begin FAnimInstanceProxy
	virtual void Initialize(UAnimInstance* InAnimInstance) override;
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	virtual void Update(float DeltaSeconds) override;
	//~End FAnimInstanceProxy

private:
	UPROPERTY(Transient, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FVector2D MoveDir;
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
