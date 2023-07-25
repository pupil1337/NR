// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Task/NRAT_ServerWaitClientTargetData.h"

#include "AbilitySystemComponent.h"

UNRAT_ServerWaitClientTargetData* UNRAT_ServerWaitClientTargetData::ServerWaitClientTargetData(UGameplayAbility* OwningAbility, bool TriggerOnce)
{
	UNRAT_ServerWaitClientTargetData* MyObj = NewAbilityTask<UNRAT_ServerWaitClientTargetData>(OwningAbility);
	MyObj->bTriggerOnce = TriggerOnce;
	return MyObj;
}

void UNRAT_ServerWaitClientTargetData::Activate()
{
	if (Ability && Ability->GetCurrentActorInfo()->IsNetAuthority() && AbilitySystemComponent.IsValid())
	{
		const FGameplayAbilitySpecHandle& SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey& ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).AddUObject(this, &ThisClass::OnTargetDataReplicatedCallback);
	}
}

void UNRAT_ServerWaitClientTargetData::OnDestroy(bool bInOwnerFinished)
{
	if (Ability && AbilitySystemComponent.IsValid())
	{
		const FGameplayAbilitySpecHandle& SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey& ActivationPredictionKey = GetActivationPredictionKey();
		AbilitySystemComponent->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey).RemoveAll(this);
	}
	
	Super::OnDestroy(bInOwnerFinished);
}

void UNRAT_ServerWaitClientTargetData::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag ActivationTag)
{
	const FGameplayAbilitySpecHandle& SpecHandle = GetAbilitySpecHandle();
	const FPredictionKey& ActivationPredictionKey = GetActivationPredictionKey();
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(SpecHandle, ActivationPredictionKey);

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(Data);
	}
	
	if (bTriggerOnce)
	{
		EndTask();
	}
}

FString UNRAT_ServerWaitClientTargetData::GetDebugString() const
{
	return FString::Printf(TEXT("Server Wait Client TargetData"));
}
