// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRComponentBase.h"
#include "NRCombatComponent.generated.h"


struct FStreamableHandle;
class ANRWeaponBase;
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
	virtual void BeginDestroy() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent) override;

//~Begin This Class
public:
	void SetEquippedWeapon(ANRWeaponBase* InWeapon);
	
	FORCEINLINE bool GetIsAiming() const { return bAiming; }
	FORCEINLINE ANRWeaponBase* GetEquippedWeapon() { return EquippedWeapon; } 
	
private:
	/** 武器开枪间隔大于上次开火时间 */
	bool IsRatePassed(uint32 FireRate) const;

	void PlayMontageByName(bool bFPS, const FName& RowName) const;

	UFUNCTION(Server, Reliable)
	void Server_PlayBodyMontageByName(const FName& RowName) const;
	UFUNCTION(NetMulticast, Reliable)
	void Multi_PlayerBodyMontageByName(const FName& RowName) const;
	
	void OnFire();
	
	void FireTicking(float DeltaTime);
	
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

	UPROPERTY(Transient, Replicated)
	bool bAiming;

	UPROPERTY(Transient)
	bool bFireTicking;
	
	UPROPERTY(Transient)
	bool bHoldingFireKey;

	UPROPERTY(Transient)
	float PreShootTime;
	
	UPROPERTY(Transient)
	ANRWeaponBase* EquippedWeapon;

	TSharedPtr<FStreamableHandle> StreamableHandle;
};
