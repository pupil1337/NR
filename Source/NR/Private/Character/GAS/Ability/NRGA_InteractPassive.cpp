// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_InteractPassive.h"

#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Character/GAS/Task/NRAT_WaitInteractTarget.h"
#include "PlayerController/NRPlayerController.h"

UNRGA_InteractPassive::UNRGA_InteractPassive()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;

	bAutoActiveOnGive = true;
	NRAbilityInputID = ENRAbilityInputID::EAIID_Interact;
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
	CurrInteraction.Reset();
	
	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		if (const ANRPlayerController* PC = Cast<ANRPlayerController>(ActorInfo->PlayerController))
		{
			PC->OnLoseInteraction(Data.Get(0)->GetHitResult()->GetActor());

			if (AT_WaitInputPress.IsValid())
			{
				AT_WaitInputPress->EndTask();
				AT_WaitInputPress.Reset();
			}

			if (AT_WaitInputRelease.IsValid())
			{
				AT_WaitInputRelease->EndTask();
				AT_WaitInputRelease.Reset();
			}
		}
	}
}

void UNRGA_InteractPassive::OnFindInteraction(const FGameplayAbilityTargetDataHandle& Data)
{
	CurrInteraction = Data.Get(0)->GetHitResult()->GetActor();
	
	if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
	{
		if (const ANRPlayerController* PC = Cast<ANRPlayerController>(ActorInfo->PlayerController))
		{
			PC->OnFindInteraction(CurrInteraction.Get());

			if (!AT_WaitInputPress.IsValid())
			{
				AT_WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
				AT_WaitInputPress->OnPress.AddDynamic(this, &ThisClass::OnInputPressed);
				AT_WaitInputPress->ReadyForActivation();
			}
		}
	}
}

void UNRGA_InteractPassive::OnInputPressed(float TimeWaited)
{
	if (!AT_WaitInputRelease.IsValid())
	{
		AT_WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
		AT_WaitInputRelease->OnRelease.AddDynamic(this, &ThisClass::OnInputRelease);
		AT_WaitInputRelease->ReadyForActivation();
	}
}

void UNRGA_InteractPassive::OnInputRelease(float TimeHeld)
{
	if (CurrInteraction.IsValid())
	{
		if (!AT_WaitInputPress.IsValid())
		{
			AT_WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
			AT_WaitInputPress->OnPress.AddDynamic(this, &ThisClass::OnInputPressed);
			AT_WaitInputPress->ReadyForActivation();
		}
	}
}
