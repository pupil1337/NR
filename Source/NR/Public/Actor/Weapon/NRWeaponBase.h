// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/NRInteractionInterface.h"
#include "Types/NRWeaponTypes.h"
#include "NRWeaponBase.generated.h"

class ANRTA_LineTrace;
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

UCLASS(Abstract, Blueprintable)
class NR_API ANRWeaponBase : public AActor, public INRInteractionInterface
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	// Mesh
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMeshComponent> Mesh3P;

	// Magazine
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> Magazine1P;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> Magazine3P;

	// IronSight
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> IronSight1P;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> IronSight3P;

	// Weapon Type
	UPROPERTY(EditDefaultsOnly)
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

	void Equip();

	void UnEquip();

	// Getter
	FNRArmAnimSetRow* GetWeaponArmAnimSetRow();
	FNRBodyAnimSetRow* GetWeaponBodyAnimSetRow();
	FNRWeaponSettingRow* GetWeaponSettingRow();
	FNRMagazineSettingRow* GetMagazineSettingRow();
	FNRIronSightSettingRow* GetIronSightSettingRow();
	
	ANRTA_LineTrace* GetLineTraceTargetActor();
	
private:
	UFUNCTION()
	void OnRep_WeaponState(ENRWeaponState OldWeaponState);

	void SetFPS_SeparateFOV(bool bInSeparateFOV, bool bInSeparate);
	
	void TickFPS_SeparateFOVDirty();
	
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

	UPROPERTY(Transient)
	ANRTA_LineTrace* TA_LineTrace;

	// Temp
	uint8 bFPS_SeparateFOVDirty : 1;
	uint8 bSeparateFOV : 1;
	uint8 bSeparate : 1;
};
