// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/NRWeapon.h"

const FName NAME_Separate_FOV_Alpha(TEXT("Separate_FOV Alpha"));
const FName NAME_Separate_Alpha(TEXT("Separate Alpha"));

ANRWeapon::ANRWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	bReplicates = true;

	// Mesh
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("骨骼网格体"));
	SetRootComponent(Mesh);
}

void ANRWeapon::BeginPlay()
{
	Super::BeginPlay();
}

void ANRWeapon::SetFPS_SeparateFOV(bool bEnable, bool bSeparate /* =false */)
{
	float SeparateFOVAlpha = bEnable ? 1.0f : 0.0f;
	float SeparateAlpha = bSeparate ? 0.1f : 1.0f;

	Mesh->SetScalarParameterValueOnMaterials(NAME_Separate_FOV_Alpha, SeparateFOVAlpha);
	Mesh->SetScalarParameterValueOnMaterials(NAME_Separate_Alpha, SeparateAlpha);
	// Mesh->SetCastShadow(!bSeparate); TODO:引擎有bug bSelfShadowOnly暂不可用
}

FNRWeaponInformationRow* ANRWeapon::GetWeaponInformation()
{
	if (!WeaponInformation.IsNull())
	{
		return WeaponInformation.GetRow<FNRWeaponInformationRow>(TEXT("ANRWeapon::GetWeaponInformation()"));	
	}
	return nullptr;
}

