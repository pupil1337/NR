// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRComponentBase.h"
#include "NRInteractionComponent.generated.h"


class UInputAction;
class UInputMappingContext;
class ANRWeaponBase;

UCLASS(Abstract, Blueprintable)
class NR_API UNRInteractionComponent : public UNRComponentBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputMappingContext> IMC_Interaction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_E;

public:
	UNRInteractionComponent();

protected:
	virtual void BeginPlay() override;
	virtual void InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

//~Begin This Class
private:
	void LookWeapon(AActor* HitActor);

	UFUNCTION()
	void StartInteractInput();

	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* Actor);
	
	UPROPERTY(Transient)
	AActor* LookingActor;
};
