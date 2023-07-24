// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/NRAbilitySystemComponent.h"


FGameplayAbilitySpecHandle UNRAbilitySystemComponent::FindAbilitySpecHandleForClass(const TSubclassOf<UGameplayAbility>& AbilityClass)
{
	ABILITYLIST_SCOPE_LOCK()
	for (const FGameplayAbilitySpec& Spec: ActivatableAbilities.Items)
	{
		if (Spec.Ability->GetClass() == AbilityClass)
		{
			return Spec.Handle;
		}
	}

	return FGameplayAbilitySpecHandle();
}
