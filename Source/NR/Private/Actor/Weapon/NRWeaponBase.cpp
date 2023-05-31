﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/NRWeaponBase.h"

#include "Types/NRWeaponTypes.h"

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

void ANRWeaponBase::SetWeaponState(ENRWeaponState InWeaponState)
{
	const ENRWeaponState OldWeaponState = WeaponState;
	WeaponState = InWeaponState;
	OnRep_WeaponState(OldWeaponState);
}

void ANRWeaponBase::OnRep_WeaponState(ENRWeaponState OldWeaponState)
{
	if (GetNetMode() != NM_DedicatedServer)
	{
		switch (WeaponState)
		{
			case ENRWeaponState::EWS_Pickup:
			{
					
			}

			default:
			{
					
			}
		}
	}
}

