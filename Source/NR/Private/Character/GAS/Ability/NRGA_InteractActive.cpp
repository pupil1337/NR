// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_InteractActive.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Interface/NRInteractInterface.h"

UNRGA_InteractActive::UNRGA_InteractActive()
{
	bActivateOnInput = false;
	NRAbilityInputID = ENRAbilityInputID::EAIID_Interact;
}

void UNRGA_InteractActive::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (ensureAlways(TriggerEventData) &&
		ensureAlways(TriggerEventData->TargetData.Get(0)) &&
		ensureAlways(TriggerEventData->TargetData.Get(0)->GetHitResult()) &&
		ensureAlways(TriggerEventData->TargetData.Get(0)->GetHitResult()->GetActor()) &&
		ensureAlways(TriggerEventData->TargetData.Get(0)->GetHitResult()->GetActor()->Implements<UNRInteractInterface>()))
	{
		// TODO
		// if (INRInteractInterface* InteractInterface = Cast<INRInteractInterface>(TriggerEventData->TargetData.Get(0)->GetHitResult()->GetActor()))
		// {
		// 	
		// }

		if (!AT_WaitInputRelease.IsValid())
		{
			AT_WaitInputRelease = UAbilityTask_WaitInputRelease::WaitInputRelease(this);
			AT_WaitInputRelease->OnRelease.AddDynamic(this, &ThisClass::OnInputRelease);
			AT_WaitInputRelease->ReadyForActivation();
		}
	}
}

void UNRGA_InteractActive::OnInputRelease(float TimeHeld)
{
	
}
