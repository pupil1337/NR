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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent) override;

//~Begin This Class
public:
	FORCEINLINE bool GetIsAiming() const { return bAiming; }
	
private:
	// Fire
	UFUNCTION()
	void StartFireInput();
	
	UFUNCTION()
	void EndFireInput();

	// Aim
	UFUNCTION()
	void StartAimInput();
	
	UFUNCTION()
	void EndAimInput();

	// TODO
	void OnAim(bool bAim);

	// TODO
	UFUNCTION(Server, Reliable)
	void Server_OnAim(bool bAim);

	UPROPERTY(Replicated)
	bool bAiming;
};
