// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRComponentBase.h"
#include "InputActionValue.h"
#include "NRRunSkiComponent.generated.h"

class UInputComponent;

/**
 * 奔跑/滑铲
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRRunSkiComponent : public UNRComponentBase
{
	GENERATED_BODY()
	
public:
	UNRRunSkiComponent();

//~Begin This Class
protected:
	virtual void InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent) override;
	
private:
	// Input
	UFUNCTION()
	void OnMoveInput(const FInputActionValue& Value);
	UFUNCTION()
	void OnRunInput();
	UFUNCTION()
	void OnCrouchInput();

	bool CheckCanRun() const;
	void Run(bool NewRun) const;
	
	// Temp
	FVector2D MoveInputValue;
};
