// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_InteractPassive.h"

#include "Character/GAS/Task/NRAT_WaitInteractTarget.h"
#include "PlayerController/NRPlayerController.h"

UNRGA_InteractPassive::UNRGA_InteractPassive()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	bAutoActiveOnGive = true;
}

void UNRGA_InteractPassive::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UNRAT_WaitInteractTarget* AT_WaitInteractTarget = UNRAT_WaitInteractTarget::WaitInteractTarget(this);
	AT_WaitInteractTarget->LoseTarget.AddDynamic(this, &ThisClass::OnLoseInteraction);
	AT_WaitInteractTarget->FindTarget.AddDynamic(this, &ThisClass::OnFindInteraction);
	AT_WaitInteractTarget->ReadyForActivation();
}

void UNRGA_InteractPassive::OnLoseInteraction(const FGameplayAbilityTargetDataHandle& Data)
{
	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		if (ANRPlayerController* PC = Cast<ANRPlayerController>(ActorInfo->PlayerController))
		{
			PC->OnLoseInteraction(Data);
		}
	}
}

void UNRGA_InteractPassive::OnFindInteraction(const FGameplayAbilityTargetDataHandle& Data)
{
	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		if (ANRPlayerController* PC = Cast<ANRPlayerController>(ActorInfo->PlayerController))
		{
			PC->OnFindInteraction(Data);
		}
	}
}
