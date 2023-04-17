// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRComponentBase.h"
#include "NRRunSkiComponent.generated.h"


/**
 * 奔跑
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRRunSkiComponent : public UNRComponentBase
{
	GENERATED_BODY()
	
public:
	UNRRunSkiComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

//~Begin This Class
protected:
	virtual void InitLocallyControlledInputEvent() override;

public:
	// Gitter
	FORCEINLINE bool IsRunning() const { return bRunning; }
	
private:
	// Input
	UFUNCTION()
	void OnMoveInput(const FInputActionValue& Value);
	UFUNCTION()
	void OnRunInput();
	UFUNCTION()
	void OnCrouchInput();

	bool CheckCanRun() const;
	void Run(bool NewRun);

	UPROPERTY(Transient, Replicated)
	bool bRunning;
	
	// Temp
	FVector2D MoveInputValue;
};
