// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Component/NRComponentBase.h"
#include "NRBagComponent.generated.h"

class UInputMappingContext;
class UInputAction;
class UInputComponent;
class ANRWeapon;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRBagComponent : public UNRComponentBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputMappingContext> IMC_Bag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_3;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_4;

	// TODO:换成从存档中读取武器数据,然后生成武器
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|临时")
	TSubclassOf<ANRWeapon> WeaponClass;
	
public:
	UNRBagComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent) override;

//~Begin This Class
public:
	UFUNCTION()
	void TryEquipWeaponInSlot(uint8 Slot);

	// Getter
	FORCEINLINE ANRWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

private:
	UFUNCTION()
	void OnRep_EquippedWeapon(const ANRWeapon* OldEquippedWeapon) const;
	
	UPROPERTY(Transient, Replicated)
	ANRWeapon* WeaponSlot[4];
	UPROPERTY(Transient, ReplicatedUsing=OnRep_EquippedWeapon)
	ANRWeapon* EquippedWeapon;
};
