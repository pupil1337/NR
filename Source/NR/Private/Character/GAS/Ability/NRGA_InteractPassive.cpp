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

	if (ActorInfo)
	{
		if (ANRPlayerController* PC = Cast<ANRPlayerController>(ActorInfo->PlayerController))
		{
			UNRAT_WaitInteractTarget* AT_WaitInteractTarget = UNRAT_WaitInteractTarget::WaitInteractTarget(this);
			AT_WaitInteractTarget->LoseTarget.AddDynamic(PC, &ANRPlayerController::OnLoseInteraction);
			AT_WaitInteractTarget->FindTarget.AddDynamic(PC, &ANRPlayerController::OnFindInteraction);
			AT_WaitInteractTarget->ReadyForActivation();		
		}
	}
}
