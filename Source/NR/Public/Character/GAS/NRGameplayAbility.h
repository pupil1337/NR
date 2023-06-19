// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "NRGASTypes.h"
#include "NRGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class NR_API UNRGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	ENRAbilityInputID AbilityInputID = ENRAbilityInputID::IA_None;

//~Begin This Class
public:
	FORCEINLINE ENRAbilityInputID GetAbilityInputID() const { return AbilityInputID; }
};
