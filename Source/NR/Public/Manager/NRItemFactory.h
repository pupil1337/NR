// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NRItemFactory.generated.h"

/**
 * 
 */
UCLASS()
class NR_API UNRItemFactory : public UObject
{
	GENERATED_BODY()
	
	// Weapon Factory
	// todo
	UFUNCTION(BlueprintCallable)
	static void SpawnWeapon(UObject* ContextObject, const FVector& Location);
};
