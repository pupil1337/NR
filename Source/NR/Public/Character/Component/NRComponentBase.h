// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NRComponentBase.generated.h"

class ANRCharacter;

UCLASS(Abstract)
class NR_API UNRComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UNRComponentBase();
	virtual void InitializeComponent() override;

//~Begin This Class
	virtual void InitLocallyControlledInputEvent() {}

protected:
	UPROPERTY(Transient)
	ANRCharacter* NRCharacter;
};