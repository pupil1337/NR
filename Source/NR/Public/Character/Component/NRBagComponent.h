// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Component/NRComponentBase.h"
#include "Types/NRWeaponTypes.h"
#include "NRBagComponent.generated.h"

class UInputMappingContext;
class UInputAction;
class UInputComponent;
class ANRWeaponBase;

USTRUCT()
struct NR_API FNRInventory
{
	GENERATED_BODY()

	TArray<ANRWeaponBase*> Weapons;
};

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRBagComponent : public UNRComponentBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputMappingContext> IMC_Bag;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_1;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_2;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_3;
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"), Category="配置|输入")
	TObjectPtr<UInputAction> IA_4;
	
public:
	UNRBagComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void PawnClientRestart() override;
	virtual void InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent) override;

//~Begin This Class
public:
	FORCEINLINE ANRWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

private:
	/**
	 * @brief 客户端发送存档给服务器, 服务器初始化仓库
	 * @todo 实现
	 */
	UFUNCTION(Server, Reliable)
	void Server_InitInventory();
	void Server_InitInventory_Implementation();
	bool Server_InitInventory_Validata();
	
	/**
	 * @brief 增加武器到仓库 仅服务器调用
	 * @param NewWeapon 需要增加的武器
	 * @param bDoEquip 是否执行装备
	 */
	void AddWeaponToInventory(ANRWeaponBase* NewWeapon, bool bDoEquip = false);
	
	UFUNCTION()
	void OnRep_Inventory();

	void EquipWeapon(ANRWeaponBase* NewWeapon);
	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(ANRWeaponBase* NewWeapon);
	void Server_EquipWeapon_Implementation(ANRWeaponBase* NewWeapon);
	bool Server_EquipWeapon_Validata(ANRWeaponBase* NewWeapon);

	void SetCurrentWeapon(ANRWeaponBase* WeaponToEquip);

	void UnEquipWeapon(ANRWeaponBase* WeaponToUnEquip);

	UFUNCTION()
	void OnRep_CurrentWeapon(ANRWeaponBase* OldWeapon);

	void TryEquipWeaponInSlot(uint8 Slot);

	// Utils
	bool IsWeaponExistInInventory(ANRWeaponBase* InWeapon);
	
	UPROPERTY(Transient, ReplicatedUsing=OnRep_Inventory)
	FNRInventory Inventory;
	
	UPROPERTY(Transient)
	ANRWeaponBase* WeaponSlot[5]; // 武器插槽 0为空手, 1~4为武器
	UPROPERTY(Transient, ReplicatedUsing=OnRep_CurrentWeapon)
	ANRWeaponBase* CurrentWeapon;
};
