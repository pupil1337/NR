// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/NRWeaponBase.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NRGameSingleton.h"
#include "Net/UnrealNetwork.h"
#include "Types/NRCollisionTypes.h"
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

	DOREPLIFETIME_CONDITION(ANRWeaponBase, WeaponState, COND_None);
}

void ANRWeaponBase::BeginPlay()
{
	Super::BeginPlay();
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
	switch (WeaponState)
	{
		case ENRWeaponState::EWS_None: break;

		case ENRWeaponState::EWS_Pickup:
		{
				// 打开碰撞
				Mesh->SetCollisionProfileName(NRCollisionProfile::Pickup_ProfileName);
				Mesh->SetSimulatePhysics(true);
				
				if (GetNetMode() != NM_DedicatedServer)
				{
					// 打开拾取提示特效
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
				break;
		}

		case ENRWeaponState::EWS_Equip:
		{
				// 关闭碰撞
				Mesh->SetSimulatePhysics(false);
				Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
				
				if (GetNetMode() != NM_DedicatedServer)
				{
					// 关闭拾取提示特效
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
				break;
		}
	default: ;
	}
}

