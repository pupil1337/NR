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
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
