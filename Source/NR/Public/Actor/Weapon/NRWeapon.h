// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Table/Weapon/NRWeaponInformation.h"
#include "NRWeapon.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;

UCLASS()
class NR_API ANRWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	FDataTableRowHandle WeaponInformation;
	
public:
	ANRWeapon();
	
	virtual void BeginPlay() override;

// This Class Func
	void SetFPS_SeparateFOV(bool bEnable, bool bSeparate = false);
	FNRWeaponInformationRow* GetWeaponInformation();
};
