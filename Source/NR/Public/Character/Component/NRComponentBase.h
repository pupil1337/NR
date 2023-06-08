// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NRComponentBase.generated.h"

class UInputComponent;
class ANRCharacter;

UCLASS(Abstract)
class NR_API UNRComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UNRComponentBase();
	virtual void InitializeComponent() override;

//~Begin This Class
	/** Character执行 Restart后立即执行 (仅在本地控制端调用) */
	virtual void PawnClientRestart() {}
	
	/** Character初始化InputComponent立即执行 (仅在本地控制端调用) */
	virtual void InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent) {}

protected:
	UPROPERTY(Transient)
	ANRCharacter* NRCharacter;
};