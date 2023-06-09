// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRComponentBase.h"
#include "NRCombatComponent.generated.h"


class UInputAction;
class UInputMappingContext;

UCLASS(Abstract, Blueprintable)
class NR_API UNRCombatComponent : public UNRComponentBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputMappingContext> IMC_Combat;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_Fire;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_Aim;
	
public:
	UNRCombatComponent();

protected:
	virtual void InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent) override;

//~Begin This Class
private:
	UFUNCTION()
	void StartFireInput();
	
	UFUNCTION()
	void EndFireInput();

	UFUNCTION()
	void StartAimInput();

	UFUNCTION()
	void EndAimInput();
};
