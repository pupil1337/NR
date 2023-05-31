// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Table/Weapon/NRWeaponInformation.h"
#include "Types/NRWeaponTypes.h"
#include "NRWeaponBase.generated.h"

class USkeletalMeshComponent;

UCLASS(Abstract, NotBlueprintable)
class NR_API ANRWeaponBase : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;
	
public:
	ANRWeaponBase();
	
	virtual void BeginPlay() override;

// This Class Func
	void SetFPS_SeparateFOV(bool bEnable, bool bSeparate = false) const;

	void SetWeaponInformation(FNRWeaponInformationRow* InWeaponInfo) { WeaponInformation = InWeaponInfo; }
	FNRWeaponInformationRow* GetWeaponInformation() const { return WeaponInformation; }

	void SetWeaponState(ENRWeaponState InWeaponState);

protected:
	UFUNCTION()
	void OnRep_WeaponState(ENRWeaponState OldWeaponState);
	
private:
	FNRWeaponInformationRow* WeaponInformation;

	UPROPERTY(ReplicatedUsing=OnRep_WeaponState)
	ENRWeaponState WeaponState = ENRWeaponState::EWS_None;
};
