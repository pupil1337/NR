// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "Types/NRGASTypes.h"
#include "NRGameplayAbility.generated.h"

/**
 * 
 */
UCLASS(Abstract, NotBlueprintable)
class NR_API UNRGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UNRGameplayAbility();
	
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	
//~Begin This Class
public:
	FORCEINLINE ENRAbilityInputID GetAbilityInputID() const { return NRAbilityInputID; }

protected:
	UPROPERTY(VisibleDefaultsOnly)
	ENRAbilityInputID NRAbilityInputID = ENRAbilityInputID::EAIID_None;

	UPROPERTY(VisibleDefaultsOnly, DisplayName="自动激活")
	bool bAutoActiveOnGive = false;
};
