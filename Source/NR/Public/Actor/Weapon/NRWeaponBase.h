// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Table/Weapon/NRWeaponInformation.h"
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

	void SetWeaponInformation(FNRWeaponInformationRow* InWeaponInfo);
	FNRWeaponInformationRow* GetWeaponInformation() const;
	
private:
	FNRWeaponInformationRow* WeaponInformation;
};
