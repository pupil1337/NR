// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "NRLifeUserWidget.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRLifeUserWidget : public UNRUserWidgetBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Health;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Shield;

public:
	virtual void BindWidgetEvent() override;

	//~Begin This Class
	void UpdateHealth(float OldHealth, float NewHealth, float MaxHealth) const;

	void UpdateMaxHealth(float OldMaxHealth, float NewMaxHealth, float Health) const;

	void UpdateShield(float OldShield, float NewShield, float MaxShield) const;

	void UpdateMaxShield(float OldMaxShield, float NewMaxShield, float Shield) const;
};
