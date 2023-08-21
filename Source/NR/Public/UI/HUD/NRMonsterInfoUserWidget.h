// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "NRMonsterInfoUserWidget.generated.h"

class UProgressBar;

/**
 * 
 */
UCLASS()
class NR_API UNRMonsterInfoUserWidget : public UNRUserWidgetBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Health;
};
