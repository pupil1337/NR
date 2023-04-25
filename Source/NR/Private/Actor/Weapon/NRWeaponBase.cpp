// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/NRWeaponBase.h"

const FName NAME_Separate_FOV_Alpha(TEXT("Separate_FOV Alpha"));
const FName NAME_Separate_Alpha(TEXT("Separate Alpha"));

ANRWeaponBase::ANRWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

	// Mesh
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("骨骼网格体"));
	SetRootComponent(Mesh);
}

void ANRWeaponBase::BeginPlay()
{
	Super::BeginPlay();
}

void ANRWeaponBase::SetFPS_SeparateFOV(bool bEnable, bool bSeparate /* =false */) const
{
	const float SeparateFOVAlpha = bEnable ? 1.0f : 0.0f;
	const float SeparateAlpha = bSeparate ? 0.1f : 1.0f;

	Mesh->SetScalarParameterValueOnMaterials(NAME_Separate_FOV_Alpha, SeparateFOVAlpha);
	Mesh->SetScalarParameterValueOnMaterials(NAME_Separate_Alpha, SeparateAlpha);
	// Mesh->SetCastShadow(!bSeparate); TODO:引擎有bug bSelfShadowOnly暂不可用
}

FNRWeaponInformationRow* ANRWeaponBase::GetWeaponInformation() const
{
	if (!WeaponInformation.IsNull())
	{
		return WeaponInformation.GetRow<FNRWeaponInformationRow>(TEXT("ANRWeapon::GetWeaponInformation()"));	
	}
	return nullptr;
}

