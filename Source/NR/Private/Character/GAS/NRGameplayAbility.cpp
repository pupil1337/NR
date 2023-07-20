// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/NRGameplayAbility.h"

#include "AbilitySystemComponent.h"

void UNRGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (bAutoActiveOnGive)
	{
		ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}
