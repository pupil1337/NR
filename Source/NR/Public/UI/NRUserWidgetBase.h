// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NRUserWidgetBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, NotBlueprintable)
class NR_API UNRUserWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual bool Initialize() override;

//~Begin This Class
protected:
	virtual void BindWidgetEvent() {}
};
