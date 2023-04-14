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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	float MaxRunSpeed = 600.0f;
	
public:
	UNRRunSkiComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

//~Begin This Class
protected:
	virtual void InitLocallyControlledInputEvent() override;

public:
	// Gitter
	FORCEINLINE float GetMaxRunSpeed() const { return MaxRunSpeed; }
	FORCEINLINE bool IsRunning() const { return bRunning; }
	
private:
	// Input
	UFUNCTION()
	void OnMoveInput(const FInputActionValue& Value);
	UFUNCTION()
	void OnRunInput();
	UFUNCTION()
	void OnCrouchInput();

	// Check
	bool CheckCanRun() const;
	
	void Run(bool NewRun);
	void RunImpl(bool NewRun);
	UFUNCTION(Server, Reliable)
	void Server_Run(bool NewRun);

	UPROPERTY(Transient, Replicated)
	bool bRunning;
	
	// Temp
	FVector2D MoveInputValue;
};
