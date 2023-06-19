// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/NRWeaponBase.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NRGameSingleton.h"
#include "Static/NRStatics.h"
#include "Net/UnrealNetwork.h"
#include "Types/NRCollisionTypes.h"
#include "Types/NRWeaponTypes.h"

ANRWeaponBase::ANRWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bReplicates = true;

	// Mesh
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("枪身"));
	SetRootComponent(Mesh);

	// Magazine
	Magazine = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("弹夹"));
	Magazine->SetupAttachment(Mesh, NAME_Socket_Magazine);

	// IronSight
	IronSight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("机瞄"));
	IronSight->SetupAttachment(Mesh, NAME_Socket_Default);
}

void ANRWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	TArray<FSoftObjectPath> TargetsToStream;
	if (GetMagazineSettingRow()) UNRStatics::AddSoftObjectPathToArray(MagazineSetting->Mesh, TargetsToStream);
	if (GetIronSightSettingRow()) UNRStatics::AddSoftObjectPathToArray(IronSightSetting->Mesh, TargetsToStream);

	const TDelegate<void()> DelegateToCall = FStreamableDelegate::CreateLambda([this]()
		{
			if (MagazineSetting && MagazineSetting->Mesh.IsValid())
			{
				Magazine->SetStaticMesh(MagazineSetting->Mesh.Get());
			}
			if (IronSightSetting && IronSightSetting->Mesh.IsValid())
			{
				IronSight->SetStaticMesh(IronSightSetting->Mesh.Get());
			}
		}
	);
	UNRStatics::RequestAsyncLoad(AttachmentStreamableHandle, TargetsToStream, DelegateToCall);
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

void ANRWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TickFPS_SeparateFOVDirty();
}

void ANRWeaponBase::Destroyed()
{
	if (PickupVfxStreamableHandle.IsValid())
	{
		PickupVfxStreamableHandle->ReleaseHandle();
		PickupVfxStreamableHandle.Reset();
	}
	if (AttachmentStreamableHandle.IsValid())
	{
		AttachmentStreamableHandle->ReleaseHandle();
		AttachmentStreamableHandle.Reset();
	}
	Super::Destroyed();
}

void ANRWeaponBase::SetWeaponState(ENRWeaponState InWeaponState)
{
	const ENRWeaponState OldWeaponState = WeaponState;
	WeaponState = InWeaponState;
	OnRep_WeaponState(OldWeaponState);
}

void ANRWeaponBase::SetOnlySeeShadow(bool bOnlyShadow) const
{
	Mesh->SetRenderInMainPass(!bOnlyShadow);
	
	Magazine->SetVisibility(!bOnlyShadow);
	Magazine->SetCastHiddenShadow(bOnlyShadow);
	
	IronSight->SetVisibility(!bOnlyShadow);
	IronSight->SetCastHiddenShadow(bOnlyShadow);
}

void ANRWeaponBase::SetSelfShadowOnly(bool bSelfShadowOnly) const
{
	Mesh->bSelfShadowOnly = bSelfShadowOnly;
	Mesh->MarkRenderStateDirty();
	
	Magazine->bSelfShadowOnly = bSelfShadowOnly;
	Magazine->MarkRenderStateDirty();
	
	IronSight->bSelfShadowOnly = bSelfShadowOnly;
	IronSight->MarkRenderStateDirty();
}

void ANRWeaponBase::SetFPS_SeparateFOV(bool bInSeparateFOV, bool bInSeparate)
{
	bFPS_SeparateFOVDirty = true;
	
	bSeparateFOV = bInSeparateFOV;
	bSeparate = bInSeparate;
}

void ANRWeaponBase::TickFPS_SeparateFOVDirty()
{
	if (bFPS_SeparateFOVDirty)
	{
		UNRStatics::SetFPS_SeparateFOV(Mesh, bSeparateFOV, bSeparate);
		UNRStatics::SetFPS_SeparateFOV(Magazine, bSeparateFOV, bSeparate);
		UNRStatics::SetFPS_SeparateFOV(IronSight, bSeparateFOV, bSeparate);
		bFPS_SeparateFOVDirty = Mesh->GetMaterials().Num() == 0 ||
								Magazine->GetMaterials().Num() == 0 ||
								IronSight->GetMaterials().Num() == 0;
	}
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
							const TDelegate<void()> DelegateToCall = FStreamableDelegate::CreateLambda([this, NRGameSingleton]()
								{
									if (NRGameSingleton->CommonVFX.PickupVFX.IsValid())
									{
										PickupNiagaraComp = NewObject<UNiagaraComponent>(this);
										PickupNiagaraComp->RegisterComponent();
										PickupNiagaraComp->SetAsset(NRGameSingleton->CommonVFX.PickupVFX.Get());
										PickupNiagaraComp->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);		
									}
								}
							);

							TArray<FSoftObjectPath> TargetsToStream;
							UNRStatics::AddSoftObjectPathToArray(NRGameSingleton->CommonVFX.PickupVFX, TargetsToStream);
							UNRStatics::RequestAsyncLoad(PickupVfxStreamableHandle, TargetsToStream, DelegateToCall);
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
						PickupVfxStreamableHandle.Reset();
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

FNRWeaponInformationRow* ANRWeaponBase::GetWeaponInformation()
{
	if (!WeaponInformation)
	{
		WeaponInformation = UNRStatics::GetWeaponInformationRow(WeaponType);
	}

	return WeaponInformation;
}

FNRArmAnimSetRow* ANRWeaponBase::GetWeaponArmAnimSetRow()
{
	if (!WeaponArmAnimSet)
	{
		if (const FNRWeaponInformationRow* WeaponInfo = GetWeaponInformation())
		{
			WeaponArmAnimSet = WeaponInfo->GetArmAnimSet();
		}
	}

	return WeaponArmAnimSet;
}

FNRBodyAnimSetRow* ANRWeaponBase::GetWeaponBodyAnimSetRow()
{
	if (!WeaponBodyAnimSet)
	{
		if (const FNRWeaponInformationRow* WeaponInfo = GetWeaponInformation())
		{
			WeaponBodyAnimSet = WeaponInfo->GetBodyAnimSet();
		}
	}

	return WeaponBodyAnimSet;
}

FNRWeaponSettingRow* ANRWeaponBase::GetWeaponSettingRow()
{
	if (!WeaponSetting)
	{
		if (const FNRWeaponInformationRow* WeaponInfo = GetWeaponInformation())
		{
			WeaponSetting = WeaponInfo->GetWeaponSetting();
		}
	}

	return WeaponSetting;
}

UDataTable* ANRWeaponBase::GetWeaponMontageDT()
{
	if (const FNRWeaponInformationRow* WeaponInfo = GetWeaponInformation())
	{
		return WeaponInfo->DT_Montage;
	}
	return nullptr;
}

UAnimMontage* ANRWeaponBase::GetWeaponMontage(bool bFPS, const FName& RowName)
{
	if (const UDataTable* DT = GetWeaponMontageDT())
	{
		if (const FNRMontageRow* MontageRow = DT->FindRow<FNRMontageRow>(RowName, RowName.ToString()))
		{
			return bFPS ? MontageRow->FPS.Get() : MontageRow->TPS.Get();	
		}
	}
	return nullptr;
}

FNRMagazineSettingRow* ANRWeaponBase::GetMagazineSettingRow()
{
	if (!MagazineSetting)
	{
		if (const FNRWeaponInformationRow* WeaponInfo = GetWeaponInformation())
		{
			MagazineSetting = WeaponInfo->GetMagazineSetting();
		}
	}
	return MagazineSetting;
}

FNRIronSightSettingRow* ANRWeaponBase::GetIronSightSettingRow()
{
	if (!IronSightSetting)
	{
		if (const FNRWeaponInformationRow* WeaponInfo = GetWeaponInformation())
		{
			IronSightSetting = WeaponInfo->GetIronSightSetting();
		}
	}
	return IronSightSetting;
}

