// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "NRAT_WaitTargetDataUsingActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitTargetDataUsingActorDelegate, const FGameplayAbilityTargetDataHandle&, Data);

class AGameplayAbilityTargetActor;

/**
 * 参考AbilityTask_WaitTargetData.h进行重写
 */
UCLASS()
class NR_API UNRAT_WaitTargetDataUsingActor : public UAbilityTask
{
	GENERATED_BODY()

protected:
	virtual void Activate() override;

public:
	virtual void ExternalConfirm(bool bEndTask) override;

	virtual void ExternalCancel() override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

//~Begin This Class
	static UNRAT_WaitTargetDataUsingActor* WaitTargetDataUsingActor(UGameplayAbility* OwningAbility, TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType, AGameplayAbilityTargetActor* TargetActor, bool bCreateKeyIfNotValidForMorePredicting = false);

	FWaitTargetDataUsingActorDelegate ValidData;

	FWaitTargetDataUsingActorDelegate Cancelled;
private:
	void InitializeTargetActor();
	void FinalizeTargetActor();
	void RegisterTargetDataCallbacks();

	UFUNCTION()
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);

	UFUNCTION()
	void OnTargetDataReplicatedCancelledCallback();

	UFUNCTION()
	void OnTargetDataReadyCallback(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnTargetDataCancelledCallback(const FGameplayAbilityTargetDataHandle& Data);
	
	bool ShouldReplicateDataToServer() const;
	
private:
	UPROPERTY(Transient)
	AGameplayAbilityTargetActor* TargetActor;

	bool bCreateKeyIfNotValidForMorePredicting;

	TEnumAsByte<EGameplayTargetingConfirmation::Type> ConfirmationType;
};
