// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h" // Only For Use FMontageWaitSimpleDelegate
#include "NRAT_PlayMontageForMeshAndWait.generated.h"

class USkeletalMeshComponent;
class UAnimMontage;

/**
 * 参考AbilityTask_PlayMontageAndWait.h进行重写
 */
UCLASS()
class NR_API UNRAT_PlayMontageForMeshAndWait : public UAbilityTask
{
	GENERATED_BODY()

public:
	UNRAT_PlayMontageForMeshAndWait();

	virtual void ExternalCancel() override;

	virtual FString GetDebugString() const override;
	
protected:
	virtual void Activate() override;

	virtual void OnDestroy(bool AbilityEnded) override;
	
public:
//~Begin This Class
	static UNRAT_PlayMontageForMeshAndWait* PlayMontageForMeshAndWait(UGameplayAbility* OwningAbility, USkeletalMeshComponent* MeshToPlay, UAnimMontage* MontageToPlay, float Rate = 1.0f, FName StartSection = NAME_None, bool bStopWhenAbilityEnds = true, float AnimRootMotionTranslationScale = 1.f, float StartTimeSeconds = 0.f);
	
	FMontageWaitSimpleDelegate OnCompleted;
	FMontageWaitSimpleDelegate OnBlendOut;
	FMontageWaitSimpleDelegate OnInterrupted;
	FMontageWaitSimpleDelegate OnCancelled;

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle InterruptedHandle;

private:
	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageInterrupted();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	bool StopPlayingMontage();
	
	UPROPERTY(Transient)
	USkeletalMeshComponent* MeshToPlay;

	UPROPERTY(Transient)
	UAnimMontage* MontageToPlay;

	UPROPERTY()
	float Rate;

	UPROPERTY()
	FName StartSection;

	UPROPERTY()
	float AnimRootMotionTranslationScale;

	UPROPERTY()
	float StartTimeSeconds;

	UPROPERTY()
	bool bStopWhenAbilityEnds;
};
