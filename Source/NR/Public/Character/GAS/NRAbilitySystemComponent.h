// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NRAbilitySystemComponent.generated.h"


UCLASS()
class NR_API UNRAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

//~Begin This Class
public:
	FGameplayAbilitySpecHandle FindAbilitySpecHandleForClass(const TSubclassOf<UGameplayAbility>& AbilityClass);
};
