// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/Weapon/NRWeaponBase.h"

#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Character/NRCharacter.h"
#include "Character/GAS/TargetActor/NRTA_LineTrace.h"
#include "Static/NRStatics.h"
#include "Net/UnrealNetwork.h"
#include "Types/NRWeaponTypes.h"

ANRWeaponBase::ANRWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	bReplicates = true;

	// SceneRoot
	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);
	
	// Mesh1P/3P
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("枪身1P"));
	Mesh1P->SetupAttachment(SceneRoot);
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("枪身3P"));
	Mesh3P->SetupAttachment(SceneRoot);

	// Magazine1P/3P
	Magazine1P = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("弹夹1P"));
	Magazine1P->SetupAttachment(Mesh1P, NAME_Socket_Magazine);
	Magazine3P = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("弹夹3P"));
	Magazine3P->SetupAttachment(Mesh3P, NAME_Socket_Magazine);
	
	// IronSight1P/3P
	IronSight1P = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("机瞄1P"));
	IronSight1P->SetupAttachment(Mesh1P, NAME_Socket_Default);
	IronSight3P = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("机瞄3P"));
	IronSight3P->SetupAttachment(Mesh3P, NAME_Socket_Default);

	// FireTracerNiagara
	FireTracerNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("开火Tracer"));
	FireTracerNiagara->SetupAttachment(SceneRoot);
}

void ANRWeaponBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}

void ANRWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ANRWeaponBase, WeaponState, COND_None);
}

void ANRWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	SetFPS_SeparateFOV(true, true);
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

	if (TA_LineTrace)
	{
		TA_LineTrace->Destroy();
	}
	
	Super::Destroyed();
}

void ANRWeaponBase::SetWeaponState(ENRWeaponState InWeaponState)
{
	const ENRWeaponState OldWeaponState = WeaponState;
	WeaponState = InWeaponState;
	OnRep_WeaponState(OldWeaponState);
}

void ANRWeaponBase::SetFPS_SeparateFOV(bool bInSeparateFOV, bool bInSeparate)
{
	bFPS_SeparateFOVDirty = true;
	
	bSeparateFOV = bInSeparateFOV;
	bSeparate = bInSeparate;
}

void ANRWeaponBase::Equip()
{
	const ANRCharacter* NRCharacter = Cast<ANRCharacter>(GetOwner());
	if (!NRCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s Owner is not NRCharacter"), *FString(__FUNCTION__), *GetName())
		return;
	}
	
	Mesh1P->AttachToComponent(NRCharacter->GetMeshArm(), FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_Socket_Weapon);
	Mesh1P->SetSkeletalMeshAsset(Mesh3P->GetSkeletalMeshAsset());
	Magazine1P->SetStaticMesh(Magazine3P->GetStaticMesh());
	IronSight1P->SetStaticMesh(IronSight3P->GetStaticMesh());
	
	Mesh3P->AttachToComponent(NRCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_HandR_IkHandGun);
}

void ANRWeaponBase::UnEquip()
{
	Mesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
}

void ANRWeaponBase::FireTracer(UNiagaraSystem* NiagaraAsset, const FVector& ImpactPoint) const
{
	bFireTraceTrigger = !FireTracerNiagara->IsActive() ? true : !bFireTraceTrigger;
	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(GetOwner()))
	{
		if (NRCharacter->IsLocallyControlled()) FireTracerNiagara->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_Socket_Muzzle);
		else FireTracerNiagara->AttachToComponent(Mesh3P, FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_Socket_Muzzle);
		
		FireTracerNiagara->SetAsset(NiagaraAsset);
		FireTracerNiagara->SetVariableBool("User.Trigger", bFireTraceTrigger);
		TArray<FVector> ImpactPoints;
		ImpactPoints.Add(ImpactPoint);
		UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(FireTracerNiagara, FName("User.ImpactPositions"), ImpactPoints);

		FireTracerNiagara->Activate();
	}
}

void ANRWeaponBase::TickFPS_SeparateFOVDirty()
{
	if (bFPS_SeparateFOVDirty)
	{
		UNRStatics::SetFPS_SeparateFOV(Mesh1P, bSeparateFOV, bSeparate);
		UNRStatics::SetFPS_SeparateFOV(Magazine1P, bSeparateFOV, bSeparate);
		UNRStatics::SetFPS_SeparateFOV(IronSight1P, bSeparateFOV, bSeparate);
		bFPS_SeparateFOVDirty = Mesh1P->GetMaterials().Num() == 0 ||
								Magazine1P->GetMaterials().Num() == 0 ||
								IronSight1P->GetMaterials().Num() == 0;
	}
}

void ANRWeaponBase::OnRep_WeaponState(ENRWeaponState OldWeaponState)
{
	// TODO
	// switch (WeaponState)
	// {
	// 	case ENRWeaponState::EWS_None: break;
	//
	// 	case ENRWeaponState::EWS_Pickup:
	// 	{
	// 			// 打开碰撞
	// 			Mesh->SetCollisionProfileName(NRCollisionProfile::Pickup_ProfileName);
	// 			Mesh->SetSimulatePhysics(true);
	// 			
	// 			if (GetNetMode() != NM_DedicatedServer)
	// 			{
	// 				// 打开拾取提示特效
	// 				if (UNRGameSingleton* NRGameSingleton = UNRGameSingleton::Get())
	// 				{
	// 					if (!NRGameSingleton->CommonVFX.PickupVFX.IsNull())
	// 					{
	// 						const TDelegate<void()> DelegateToCall = FStreamableDelegate::CreateLambda([this, NRGameSingleton]()
	// 							{
	// 								if (NRGameSingleton->CommonVFX.PickupVFX.IsValid())
	// 								{
	// 									PickupNiagaraComp = NewObject<UNiagaraComponent>(this);
	// 									PickupNiagaraComp->RegisterComponent();
	// 									PickupNiagaraComp->SetAsset(NRGameSingleton->CommonVFX.PickupVFX.Get());
	// 									PickupNiagaraComp->AttachToComponent(Mesh, FAttachmentTransformRules::KeepRelativeTransform);		
	// 								}
	// 							}
	// 						);
	//
	// 						TArray<FSoftObjectPath> TargetsToStream;
	// 						UNRStatics::AddSoftObjectPathToArray(NRGameSingleton->CommonVFX.PickupVFX, TargetsToStream);
	// 						UNRStatics::RequestAsyncLoad(PickupVfxStreamableHandle, TargetsToStream, DelegateToCall);
	// 					}
	// 				}	
	// 			}
	// 			break;
	// 	}
	//
	// 	case ENRWeaponState::EWS_Equip:
	// 	{
	// 			// 关闭碰撞
	// 			Mesh->SetSimulatePhysics(false);
	// 			Mesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	// 			
	// 			if (GetNetMode() != NM_DedicatedServer)
	// 			{
	// 				// 关闭拾取提示特效
	// 				if (PickupVfxStreamableHandle)
	// 				{
	// 					PickupVfxStreamableHandle->ReleaseHandle();
	// 					PickupVfxStreamableHandle.Reset();
	// 				}
	// 				if (PickupNiagaraComp)
	// 				{
	// 					PickupNiagaraComp->DestroyComponent();
	// 					PickupNiagaraComp = nullptr;
	// 				}	
	// 			}
	// 			break;
	// 	}
	// default: ;
	// }
}



// Getter ================================================================================
FNRWeaponInformationRow* ANRWeaponBase::GetWeaponInformation() const
{
	if (!WeaponInformation)
	{
		WeaponInformation = UNRStatics::GetWeaponInformationRow(WeaponType);
	}

	check(WeaponInformation)
	return WeaponInformation;
}

FNRArmAnimSetRow* ANRWeaponBase::GetWeaponArmAnimSetRow() const
{
	if (!WeaponArmAnimSet)
	{
		if (const FNRWeaponInformationRow* WeaponInfo = GetWeaponInformation())
		{
			WeaponArmAnimSet = WeaponInfo->GetArmAnimSet();
		}
	}

	check(WeaponArmAnimSet)
	return WeaponArmAnimSet;
}

FNRBodyAnimSetRow* ANRWeaponBase::GetWeaponBodyAnimSetRow() const
{
	if (!WeaponBodyAnimSet)
	{
		if (const FNRWeaponInformationRow* WeaponInfo = GetWeaponInformation())
		{
			WeaponBodyAnimSet = WeaponInfo->GetBodyAnimSet();
		}
	}

	check(WeaponBodyAnimSet)
	return WeaponBodyAnimSet;
}

FNRWeaponSettingRow* ANRWeaponBase::GetWeaponSettingRow() const
{
	if (!WeaponSetting)
	{
		if (const FNRWeaponInformationRow* WeaponInfo = GetWeaponInformation())
		{
			WeaponSetting = WeaponInfo->GetWeaponSetting();
		}
	}

	check(WeaponSetting)
	return WeaponSetting;
}

ANRTA_LineTrace* ANRWeaponBase::GetLineTraceTargetActor()
{
	if (TA_LineTrace)
	{
		return TA_LineTrace;
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	TA_LineTrace = GetWorld()->SpawnActor<ANRTA_LineTrace>();
	return TA_LineTrace;
}

