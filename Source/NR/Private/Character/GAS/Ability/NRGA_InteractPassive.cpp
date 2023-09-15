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
	AT_WaitInteractTarget->UpdateTarget.AddDynamic(this, &ThisClass::OnUpdateViewTarget);
	AT_WaitInteractTarget->ReadyForActivation();
}

void UNRGA_InteractPassive::OnUpdateViewTarget(const FGameplayAbilityTargetDataHandle& Data)
{
	ViewTarget = Data;

	// 如果当前没有交互操作 则更新UI/输入
	if (!CurrInteraction.IsValid())
	{
		if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
		{
			if (const ANRPlayerController* PC = Cast<ANRPlayerController>(ActorInfo->PlayerController))
			{
				// UI
				PC->OnFindInteraction(ViewTarget.Get(0)->GetHitResult()->GetActor());

				// 输入
				if (ViewTarget.Get(0)->GetHitResult()->GetActor())
				{
					if (!AT_WaitInputPress.IsValid())
					{
						AT_WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
						AT_WaitInputPress->OnPress.AddDynamic(this, &ThisClass::OnInputPressed);
						AT_WaitInputPress->ReadyForActivation();
					}
				}
				else
				{
					if (AT_WaitInputPress.IsValid())
					{
						AT_WaitInputPress->EndTask();
						AT_WaitInputPress.Reset();
					}
				}
			}
		}
	}
}

void UNRGA_InteractPassive::OnInputPressed(float TimeWaited)
{
	// TODO
}
