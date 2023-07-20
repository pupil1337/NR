// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_Shield.h"

#include "AbilitySystemComponent.h"

UNRGA_Shield::UNRGA_Shield()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	bAutoActiveOnGive = true;
}

void UNRGA_Shield::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		}

		
		FGameplayEffectContextHandle EffectContextHandle = ActorInfo->AbilitySystemComponent->MakeEffectContext();
		EffectContextHandle.AddSourceObject(ActorInfo->AvatarActor.Get());

		// ShieldInit
		if (const FGameplayEffectSpecHandle ShieldInitSpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(ShieldInit, 1.0f, EffectContextHandle); ShieldInitSpecHandle.IsValid())
		{
			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*ShieldInitSpecHandle.Data.Get());
		}

		// ShieldRegen
		if (const FGameplayEffectSpecHandle ShieldRegenSpecHandle = ActorInfo->AbilitySystemComponent->MakeOutgoingSpec(ShieldRegen, 1.0f, EffectContextHandle); ShieldRegenSpecHandle.IsValid())
		{
			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*ShieldRegenSpecHandle.Data.Get());
		}
	}
}
