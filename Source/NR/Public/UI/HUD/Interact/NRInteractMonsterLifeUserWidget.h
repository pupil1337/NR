// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "NRInteractMonsterLifeUserWidget.generated.h"

class UProgressBar;
class ANRMonster;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRInteractMonsterLifeUserWidget : public UNRUserWidgetBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Armor;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Shield;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Health;

//~Begin This Class
public:
	void OnLoseMonster();
	
	void OnFindMonster(ANRMonster* InMonster);

private:

	void UpdateArmor(float OldArmor, float NewArmor, float MaxArmor);
	
	void UpdateMaxArmor(float OldMaxArmor, float NewMaxArmor, float Armor);

	void UpdateShield(float OldShield, float NewShield, float MaxShield);
	
	void UpdateMaxShield(float OldMaxShield, float NewMaxShield, float Shield);
	
	void UpdateHealth(float OldHealth, float NewHealth, float MaxHealth);
	
	void UpdateMaxHealth(float OldMaxHealth, float NewMaxHealth, float Health);
	
	TWeakObjectPtr<ANRMonster> Monster;

	FDelegateHandle ArmorHandle;
	FDelegateHandle MaxArmorHandle;
	FDelegateHandle ShieldHandle;
	FDelegateHandle MaxShieldHandle;
	FDelegateHandle HealthHandle;
	FDelegateHandle MaxHealthHandle;
};
