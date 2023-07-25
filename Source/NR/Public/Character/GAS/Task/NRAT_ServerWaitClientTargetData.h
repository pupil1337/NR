// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h" // Only for use 'FWaitTargetDataDelegate'
#include "NRAT_ServerWaitClientTargetData.generated.h"

/**
 * 
 */
UCLASS()
class NR_API UNRAT_ServerWaitClientTargetData : public UAbilityTask
{
	GENERATED_BODY()

protected:
	virtual void Activate() override;

public:
	virtual void OnDestroy(bool bInOwnerFinished) override;

	virtual FString GetDebugString() const override;

//~Begin This Class
	static UNRAT_ServerWaitClientTargetData* ServerWaitClientTargetData(UGameplayAbility* OwningAbility, bool TriggerOnce);
	
	FWaitTargetDataDelegate ValidData;
	
private:
	UFUNCTION()
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag);
	
	bool bTriggerOnce;
};
