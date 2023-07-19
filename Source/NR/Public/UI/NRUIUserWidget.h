// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "NRUIUserWidget.generated.h"

class UNRLifeUserWidget;
class UNRCrosshairUserWidget;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRUIUserWidget : public UNRUserWidgetBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UNRCrosshairUserWidget> CrosshairUserWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UNRLifeUserWidget> LifeUserWidget;
};
