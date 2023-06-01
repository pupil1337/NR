// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRComponentBase.h"
#include "NRInteractionComponent.generated.h"


UCLASS(Abstract, Blueprintable)
class NR_API UNRInteractionComponent : public UNRComponentBase
{
	GENERATED_BODY()

public:
	UNRInteractionComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
