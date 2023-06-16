// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/NRInteractionInterface.h"
#include "Types/NRWeaponTypes.h"
#include "NRWeaponBase.generated.h"

class UDataTable;
struct FNRWeaponSettingRow;
struct FNRArmAnimSetRow;
struct FNRWeaponInformationRow;
struct FNRBodyAnimSetRow;
struct FStreamableHandle;
class USkeletalMeshComponent;
class UNiagaraComponent;

UCLASS(Abstract, NotBlueprintable)
class NR_API ANRWeaponBase : public AActor, public INRInteractionInterface
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> Magazine;

	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"))
	ENRWeaponType WeaponType = ENRWeaponType::EWT_None;
	
public:
	ANRWeaponBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;

	// INRInteractionInterface
	virtual ENRInteractionType GetInteractionType() const override { return ENRInteractionType::EIT_Weapon; }

// This Class Func
	void SetWeaponState(ENRWeaponState InWeaponState);
	void SetVisibility(bool bRender) const;
	void SetFPS_SeparateFOV(bool bEnable, bool bSeparate) const;

	FNRArmAnimSetRow* GetWeaponArmAnimSetRow();
	FNRBodyAnimSetRow* GetWeaponBodyAnimSetRow();
	FNRWeaponSettingRow* GetWeaponSettingRow();
	UDataTable* GetWeaponMontageDT(); // FNRMontageRow
	UAnimMontage* GetWeaponMontage(bool bFPS, const FName& RowName);

	FORCEINLINE ENRWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE ENRWeaponState GetWeaponState() const { return WeaponState; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	
protected:
	UFUNCTION()
	void OnRep_WeaponState(ENRWeaponState OldWeaponState);
	
private:
	FNRWeaponInformationRow* GetWeaponInformation();
	FNRWeaponInformationRow* WeaponInformation;
	FNRArmAnimSetRow* WeaponArmAnimSet;
	FNRBodyAnimSetRow* WeaponBodyAnimSet;
	FNRWeaponSettingRow* WeaponSetting;

	UPROPERTY(ReplicatedUsing=OnRep_WeaponState)
	ENRWeaponState WeaponState = ENRWeaponState::EWS_None;

	UPROPERTY(Transient)
	UNiagaraComponent* PickupNiagaraComp;

	TSharedPtr<FStreamableHandle> PickupVfxStreamableHandle;
};
