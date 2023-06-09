// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/NRInteractionInterface.h"
#include "Table/Weapon/NRWeaponInformation.h"
#include "Types/NRWeaponTypes.h"
#include "NRWeaponBase.generated.h"

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
	FDataTableRowHandle WeaponInformationRowHandle;
	
public:
	ANRWeaponBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void BeginPlay() override;

	// INRInteractionInterface
	virtual ENRInteractionType GetInteractionType() const override { return ENRInteractionType::EIT_Weapon; }

// This Class Func
	FNRWeaponInformationRow* GetWeaponInformation();

	void SetWeaponState(ENRWeaponState InWeaponState);

	void SetRenderInMainPass(bool bRender) const;

	FORCEINLINE ENRWeaponState GetWeaponState() const { return WeaponState; }
	FORCEINLINE USkeletalMeshComponent* GetMesh() const { return Mesh; }
	
protected:
	UFUNCTION()
	void OnRep_WeaponState(ENRWeaponState OldWeaponState);
	
private:
	FNRWeaponInformationRow* WeaponInformation;

	UPROPERTY(ReplicatedUsing=OnRep_WeaponState)
	ENRWeaponState WeaponState = ENRWeaponState::EWS_None;

	UPROPERTY(Transient)
	UNiagaraComponent* PickupNiagaraComp;

	TSharedPtr<FStreamableHandle> PickupVfxStreamableHandle;
};
