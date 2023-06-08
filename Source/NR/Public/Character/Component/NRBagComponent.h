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
	
public:
	UNRBagComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
protected:
	virtual void PawnClientRestart() override;
	virtual void InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent) override;

//~Begin This Class
public:
	/** LocallyControlled Only */
	void EquipFPSWeapon(FName WeaponRowName);
	
	UFUNCTION(Server, Reliable)
	void Server_EquipTPSWeapon(FName WeaponRowName);

	UFUNCTION()
	void OnRep_TPSWeapon(ANRWeaponBase* OldWeapon);

	void TryEquipWeaponInSlot(uint8 Slot);
	
	FORCEINLINE ANRWeaponBase* GetFPSWeapon() const { return FPSWeapon; }
	FORCEINLINE ANRWeaponBase* GetTPSWeapon() const { return TPSWeapon; }

private:
	UPROPERTY(Transient)
	ANRWeaponBase* WeaponSlot[5]; // 武器插槽 0为空手, 1~4为武器
	UPROPERTY(Transient)
	ANRWeaponBase* FPSWeapon; // 1P武器 仅在本地控制端
	UPROPERTY(Transient, ReplicatedUsing=OnRep_TPSWeapon)
	ANRWeaponBase* TPSWeapon; // 3P武器 在服务端生成同步给客户端
};
