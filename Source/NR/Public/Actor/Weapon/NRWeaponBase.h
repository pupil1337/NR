// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/NRInteractionInterface.h"
#include "Types/NRWeaponTypes.h"
#include "NRWeaponBase.generated.h"

struct FNRIronSightSettingRow;
class UDataTable;
struct FNRMagazineSettingRow;
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
	TObjectPtr<UStaticMeshComponent> IronSight;
	
	UPROPERTY(EditDefaultsOnly, meta=(AllowPrivateAccess="true"))
	ENRWeaponType WeaponType = ENRWeaponType::EWT_None;
	
public:
	ANRWeaponBase();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void Destroyed() override;

	// INRInteractionInterface
	virtual ENRInteractionType GetInteractionType() const override { return ENRInteractionType::EIT_Weapon; }

// This Class Func
	void SetWeaponState(ENRWeaponState InWeaponState);
	void SetOnlySeeShadow(bool bOnlyShadow) const;
	void SetSelfShadowOnly(bool bSelfShadowOnly) const;
	void SetFPS_SeparateFOV(bool bInSeparateFOV, bool bInSeparate);

	FNRArmAnimSetRow* GetWeaponArmAnimSetRow();
	FNRBodyAnimSetRow* GetWeaponBodyAnimSetRow();
	FNRWeaponSettingRow* GetWeaponSettingRow();
	UDataTable* GetWeaponMontageDT(); // FNRMontageRow
	UAnimMontage* GetWeaponMontage(bool bFPS, const FName& RowName);
	FNRMagazineSettingRow* GetMagazineSettingRow();
	FNRIronSightSettingRow* GetIronSightSettingRow();

	FORCEINLINE ENRWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE ENRWeaponState GetWeaponState() const { return WeaponState; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	
protected:
	UFUNCTION()
	void OnRep_WeaponState(ENRWeaponState OldWeaponState);

	void TickFPS_SeparateFOVDirty();
	
private:
	FNRWeaponInformationRow* GetWeaponInformation();
	FNRWeaponInformationRow* WeaponInformation;
	FNRArmAnimSetRow* WeaponArmAnimSet;
	FNRBodyAnimSetRow* WeaponBodyAnimSet;
	FNRWeaponSettingRow* WeaponSetting;
	FNRMagazineSettingRow* MagazineSetting;
	FNRIronSightSettingRow* IronSightSetting;

	UPROPERTY(ReplicatedUsing=OnRep_WeaponState)
	ENRWeaponState WeaponState = ENRWeaponState::EWS_None;

	UPROPERTY(Transient)
	UNiagaraComponent* PickupNiagaraComp;

	TSharedPtr<FStreamableHandle> PickupVfxStreamableHandle;
	TSharedPtr<FStreamableHandle> AttachmentStreamableHandle;

	// Temp
	uint8 bFPS_SeparateFOVDirty : 1;
	uint8 bSeparateFOV : 1;
	uint8 bSeparate : 1;
};
