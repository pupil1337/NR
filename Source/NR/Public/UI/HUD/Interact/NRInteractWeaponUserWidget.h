// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "NRInteractWeaponUserWidget.generated.h"

class ANRWeaponBase;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRInteractWeaponUserWidget : public UNRUserWidgetBase
{
	GENERATED_BODY()

//~Begin This Class
public:
	void OnLoseWeapon();
	
	void OnFindWeapon(ANRWeaponBase* InWeapon);

private:
	TWeakObjectPtr<ANRWeaponBase> Weapon;
};
