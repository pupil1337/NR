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

//~Begin This Class
public:
	FORCEINLINE ENRAbilityInputID GetAbilityInputID() const { return AbilityInputID; }

protected:
	UPROPERTY(VisibleDefaultsOnly)
	ENRAbilityInputID AbilityInputID = ENRAbilityInputID::EAIID_None;
};
