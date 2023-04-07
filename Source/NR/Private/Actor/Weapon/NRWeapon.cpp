// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/NRWeapon.h"

#include "Character/NRCharacter.h"

const FName NAME_Separate_FOV_Alpha(TEXT("Separate_FOV Alpha"));

ANRWeapon::ANRWeapon()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Mesh
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("骨骼网格体"));
	Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	Mesh->CastShadow = false;
}

void ANRWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (ANRCharacter* NRCharacter = Cast<ANRCharacter>(GetOwner()))
	{
		if (NRCharacter->IsLocallyControlled())
		{
			Mesh->SetScalarParameterValueOnMaterials(NAME_Separate_FOV_Alpha, 1.0f);
		}
	}
}

