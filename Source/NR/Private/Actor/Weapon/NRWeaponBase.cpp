// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/NRWeaponBase.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NRGameSingleton.h"
#include "Net/UnrealNetwork.h"
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

void ANRWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ANRWeaponBase, WeaponState, COND_None, REPNOTIFY_Always);
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

FNRWeaponInformationRow* ANRWeaponBase::GetWeaponInformation()
{
	if (!WeaponInformation)
	{
		if (!WeaponInformationRowHandle.IsNull())
		{
			WeaponInformation = WeaponInformationRowHandle.GetRow<FNRWeaponInformationRow>(TEXT("ANRWeapon::GetWeaponInformation()"));	
		}
	}

	return WeaponInformation;
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
				if (UNRGameSingleton* NRGameSingleton = UNRGameSingleton::Get())
				{
					if (!NRGameSingleton->CommonVFX.PickupVFX.IsNull())
					{
						PickupVfxStreamableHandle = NRGameSingleton->StreamableManager.RequestAsyncLoad(NRGameSingleton->CommonVFX.PickupVFX.ToSoftObjectPath(),
							FStreamableDelegate::CreateLambda([this, NRGameSingleton]()
							{
								PickupNiagaraComp = NewObject<UNiagaraComponent>(this);
								PickupNiagaraComp->RegisterComponent();
								PickupNiagaraComp->SetAsset(NRGameSingleton->CommonVFX.PickupVFX.Get());
								PickupNiagaraComp->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);	
							})
						);
					}
				}
			}

			default:
			{
				if (PickupVfxStreamableHandle)
				{
					PickupVfxStreamableHandle->ReleaseHandle();
					PickupVfxStreamableHandle->ReleaseHandle();
				}
				if (PickupNiagaraComp)
				{
					PickupNiagaraComp->DestroyComponent();
					PickupNiagaraComp = nullptr;
				}
			}
		}
	}
}

