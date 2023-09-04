// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "NRInteractUserWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRInteractUserWidget : public UNRUserWidgetBase
{
	GENERATED_BODY()
	
public:
	
//~Begin This Class
	void OnLoseInteraction(AActor* InActor);

	void OnFindInteraction(AActor* InActor);
};
