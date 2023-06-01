// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "NRCrosshairUserWidget.generated.h"

class UImage;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRCrosshairUserWidget : public UNRUserWidgetBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Crosshair;
};
