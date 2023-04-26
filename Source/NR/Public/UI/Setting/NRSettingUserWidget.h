// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "NRSettingUserWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRSettingUserWidget : public UNRUserWidgetBase
{
	GENERATED_BODY()
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_SaveGame;

protected:
	virtual void BindWidgetEvent() override;

private:
	UFUNCTION()
	void OnButton_SaveGameClicked();
};
