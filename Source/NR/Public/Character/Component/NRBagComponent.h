// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Component/NRComponentBase.h"
#include "NRBagComponent.generated.h"

class UInputMappingContext;
class UInputAction;
class UInputComponent;
class ANRWeaponBase;

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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"), Category="配置|空手")
	TSubclassOf<ANRWeaponBase> DefaultWeaponClass;
	
public:
	UNRBagComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent) override;

//~Begin This Class
public:
	/** Server Only */
	void GetItemInWorld(AActor* Actor);

	// Weapon ================================================================================
	bool GetCanUseWeaponSlot(uint8& Slot) const;
	
	UFUNCTION()
	void TryEquipWeaponInSlot(uint8 Slot);
	
	FORCEINLINE ANRWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }

private:
	UFUNCTION()
	void OnRep_EquippedWeapon(const ANRWeaponBase* OldEquippedWeapon) const;

	void OnLocallyControlledEquipWeapon() const;
	
	UPROPERTY(Transient)
	ANRWeaponBase* WeaponSlot[5]; // 武器插槽 0为空手, 1~4为武器
	UPROPERTY(Transient, ReplicatedUsing=OnRep_EquippedWeapon)
	ANRWeaponBase* EquippedWeapon;
};
