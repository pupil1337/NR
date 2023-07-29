// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "NRInventoryComponent.generated.h"

struct FStreamableHandle;
class UInputMappingContext;
class UInputAction;
class UInputComponent;
class ANRWeaponBase;

USTRUCT()
struct NR_API FNRInventory
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<ANRWeaponBase*> Weapons;
};

/**
 * 
 */
UCLASS(Blueprintable)
class NR_API UNRInventoryComponent : public UActorComponent
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
	UNRInventoryComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnComponentDestroyed(bool bDestroyingHierarchy) override;

//~Begin This Class
	virtual void OnPawnClientRestart();
	virtual void OnSetupPlayerInputComponent(UInputComponent* PlayerInputComponent);
	
	FORCEINLINE ANRWeaponBase* GetCurrentWeapon() const { return CurrentWeapon; }

private:
	// ----------------------------------------------------------------------------------------------------------------
	// Inventory
	// ----------------------------------------------------------------------------------------------------------------
	
	/**
	 * @brief 客户端发送存档给服务器, 服务器初始化仓库
	 * @todo 实现
	 */
	UFUNCTION(Server, Reliable)
	void Server_InitInventory();
	void Server_InitInventory_Implementation();
	
	/**
	 * @brief 增加武器到仓库 仅服务器调用
	 * @param NewWeapon 需要增加的武器
	 * @param bDoEquip 是否执行装备
	 */
	void AddWeaponToInventory(ANRWeaponBase* NewWeapon, bool bDoEquip = false);
	
	UFUNCTION()
	void OnRep_Inventory();

	// ----------------------------------------------------------------------------------------------------------------
	// Weapon
	// ----------------------------------------------------------------------------------------------------------------
	
	/**
	 * @brief 装备武器: 客户端调用CurrentWeapon使用预测, 服务器调用则RPC客户端同步
	 * @param NewWeapon 需要装备的武器
	 */
	void EquipWeapon(ANRWeaponBase* NewWeapon);
	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(ANRWeaponBase* NewWeapon);
	void Server_EquipWeapon_Implementation(ANRWeaponBase* NewWeapon);

	void SetCurrentWeapon(ANRWeaponBase* NewWeapon, ANRWeaponBase* OldWeapon);

	void UnEquipWeapon(ANRWeaponBase* WeaponToUnEquip);

	UFUNCTION()
	void OnRep_CurrentWeapon(ANRWeaponBase* OldWeapon);

	UFUNCTION(Client, Reliable)
	void Client_SyncCurrentWeapon(ANRWeaponBase* InWeapon);
	void Client_SyncCurrentWeapon_Implementation(ANRWeaponBase* InWeapon);

	UFUNCTION(Server, Reliable)
	void Server_SyncCurrentWeapon();
	void Server_SyncCurrentWeapon_Implementation();
	
	void TryEquipWeaponInSlot(uint8 Slot);
	
	bool IsWeaponExistInInventory(ANRWeaponBase* InWeapon);

	// ----------------------------------------------------------------------------------------------------------------
	// Streamable
	// ----------------------------------------------------------------------------------------------------------------

	void LoadCurrentWeaponAssets();

	TSharedPtr<FStreamableHandle> StreamableHandle_CurrentWeapon;
	
	// ----------------------------------------------------------------------------------------------------------------
	// Members
	// ----------------------------------------------------------------------------------------------------------------

	/** Inventory */
	UPROPERTY(Transient, ReplicatedUsing=OnRep_Inventory)
	FNRInventory Inventory;

	/** Weapon Shortcut */
	UPROPERTY(Transient)
	ANRWeaponBase* WeaponSlot[5]; // 武器插槽 0为空手, 1~4为武器
	UPROPERTY(Transient, ReplicatedUsing=OnRep_CurrentWeapon)
	ANRWeaponBase* CurrentWeapon;
};
