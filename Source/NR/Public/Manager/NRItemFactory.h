﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NRItemFactory.generated.h"

class ANRWeaponBase;

/**
 * 
 */
UCLASS()
class NR_API UNRItemFactory : public UObject
{
	GENERATED_BODY()

public:
	
	/**
	 * @brief 通过武器RowName生成模板武器
	 * @param ContextObject Context
	 * @param WeaponRowName 武器RowName
	 * @return 生成的武器
	 */
	static ANRWeaponBase* SpawnWeapon(const UObject* ContextObject, FName WeaponRowName);
	
	// Weapon Factory
	// todo
	UFUNCTION(BlueprintCallable)
	static void SpawnWeapon(UObject* ContextObject, const FVector& Location);
};
