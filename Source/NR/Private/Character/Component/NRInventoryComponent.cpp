// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/NRInventoryComponent.h"

#include "Actor/Weapon/NRWeaponBase.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/NRCharacter.h"
#include "Manager/NRItemFactory.h"
#include "Net/UnrealNetwork.h"
#include "Static/NRStatics.h"

UNRInventoryComponent::UNRInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UNRInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UNRInventoryComponent, Inventory, COND_None)
	DOREPLIFETIME_CONDITION(UNRInventoryComponent, CurrentWeapon, COND_SimulatedOnly)
}

void UNRInventoryComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
	if (StreamableHandle_CurrentWeapon.IsValid())
	{
		StreamableHandle_CurrentWeapon.Get()->ReleaseHandle();
		StreamableHandle_CurrentWeapon.Reset();
	}
	
	Super::OnComponentDestroyed(bDestroyingHierarchy);
}

void UNRInventoryComponent::OnPawnClientRestart()
{
	Server_InitInventory();
}

void UNRInventoryComponent::OnSetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(GetOwner()))
	{
		if (const APlayerController* PlayerController = Cast<APlayerController>(NRCharacter->GetController()))
		{
			if (const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
				{
					if (IMC_Bag)
					{
						EnhancedInputLocalPlayerSubsystem->AddMappingContext(IMC_Bag, 0);
						if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
						{
							if (IA_1)
							{
								EnhancedInputComponent->BindAction(IA_1, ETriggerEvent::Started, this, &ThisClass::TryEquipWeaponInSlot, static_cast<uint8>(1));
							}
							if (IA_2)
							{
								EnhancedInputComponent->BindAction(IA_2, ETriggerEvent::Started, this, &ThisClass::TryEquipWeaponInSlot, static_cast<uint8>(2));
							}
							if (IA_3)
							{
								EnhancedInputComponent->BindAction(IA_3, ETriggerEvent::Started, this, &ThisClass::TryEquipWeaponInSlot, static_cast<uint8>(3));
							}
							if (IA_4)
							{
								EnhancedInputComponent->BindAction(IA_4, ETriggerEvent::Started, this, &ThisClass::TryEquipWeaponInSlot, static_cast<uint8>(4));
							}
						}
					}
				}
			}
		}	
	}
}

// ----------------------------------------------------------------------------------------------------------------
// Inventory

void UNRInventoryComponent::Server_InitInventory_Implementation()
{
	if (ANRCharacter* NRCharacter = Cast<ANRCharacter>(GetOwner()))
	{
		if (ANRWeaponBase* NewWeapon = UNRItemFactory::SpawnWeapon(this, ENRWeaponType::EWT_AR_01))
		{
			NewWeapon->SetOwner(NRCharacter);
			NewWeapon->SetReplicates(true);
			NewWeapon->SetWeaponState(ENRWeaponState::EWS_Equip);
			NewWeapon->AttachToActor(NRCharacter, FAttachmentTransformRules::KeepRelativeTransform);
			AddWeaponToInventory(NewWeapon, true);
		}	
	}
}

void UNRInventoryComponent::AddWeaponToInventory(ANRWeaponBase* NewWeapon, bool bDoEquip/* =false */)
{
	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(GetOwner()))
	{
		if (NRCharacter->HasAuthority())
		{
			if (IsWeaponExistInInventory(NewWeapon))
			{
				return;
			}

			Inventory.Weapons.Add(NewWeapon);

			if (bDoEquip)
			{
				EquipWeapon(NewWeapon);
				Client_SyncCurrentWeapon(CurrentWeapon);
			}
		}	
	}
}

void UNRInventoryComponent::OnRep_Inventory()
{
	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(GetOwner()))
	{
		if (NRCharacter->GetLocalRole() == ROLE_AutonomousProxy && Inventory.Weapons.Num() > 0 && !CurrentWeapon)
		{
			Server_SyncCurrentWeapon();
		}
	}
}

// ----------------------------------------------------------------------------------------------------------------
// Weapon

void UNRInventoryComponent::EquipWeapon(ANRWeaponBase* NewWeapon)
{
	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(GetOwner()))
	{
		if (NRCharacter->GetLocalRole() < ROLE_Authority)
		{
			Server_EquipWeapon(NewWeapon);
			SetCurrentWeapon(NewWeapon, CurrentWeapon);
		}
		else
		{
			SetCurrentWeapon(NewWeapon, CurrentWeapon);
		}
	}
}

void UNRInventoryComponent::Server_EquipWeapon_Implementation(ANRWeaponBase* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

void UNRInventoryComponent::SetCurrentWeapon(ANRWeaponBase* NewWeapon, ANRWeaponBase* OldWeapon)
{
	if (NewWeapon == OldWeapon)
	{
		return;
	}

	UnEquipWeapon(OldWeapon);

	if (NewWeapon)
	{
		CurrentWeapon = NewWeapon;
		CurrentWeapon->SetOwner(GetOwner());
		CurrentWeapon->Equip();
		LoadCurrentWeaponAssets();
	}
}

void UNRInventoryComponent::UnEquipWeapon(ANRWeaponBase* WeaponToUnEquip)
{
	if (WeaponToUnEquip)
	{
		WeaponToUnEquip->UnEquip();
	}
}

void UNRInventoryComponent::OnRep_CurrentWeapon(ANRWeaponBase* OldWeapon)
{
	SetCurrentWeapon(CurrentWeapon, OldWeapon);
}

void UNRInventoryComponent::Client_SyncCurrentWeapon_Implementation(ANRWeaponBase* InWeapon)
{
	ANRWeaponBase* OldWeapon = CurrentWeapon;
	CurrentWeapon = InWeapon;
	OnRep_CurrentWeapon(OldWeapon);
}

void UNRInventoryComponent::Server_SyncCurrentWeapon_Implementation()
{
	Client_SyncCurrentWeapon(CurrentWeapon);
}

void UNRInventoryComponent::TryEquipWeaponInSlot(uint8 Slot)
{
	if (Slot < 1 || Slot > 4)
	{
		return;
	}
}

bool UNRInventoryComponent::IsWeaponExistInInventory(ANRWeaponBase* InWeapon)
{
	for (const ANRWeaponBase* Weapon: Inventory.Weapons)
	{
		if (Weapon == InWeapon)
		{
			return true;
		}
	}

	return false;
}

// ----------------------------------------------------------------------------------------------------------------
// Streamable

void UNRInventoryComponent::LoadCurrentWeaponAssets()
{
	TArray<FSoftObjectPath> TargetsToStream;
	
	if (CurrentWeapon)
	{
		if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(GetOwner()))
		{
			if (NRCharacter->IsLocallyControlled())
			{
				// 1P 手臂 动画
				if (const FNRArmAnimSetRow* ArmAnimSetRow = CurrentWeapon->GetWeaponArmAnimSetRow())
				{
					// Sequence
					TargetsToStream.Add(ArmAnimSetRow->AimBreath.ToSoftObjectPath());
					TargetsToStream.Add(ArmAnimSetRow->IdleBreath.ToSoftObjectPath());
					TargetsToStream.Add(ArmAnimSetRow->AimPose.ToSoftObjectPath());
					TargetsToStream.Add(ArmAnimSetRow->AimBreath.ToSoftObjectPath());
					TargetsToStream.Add(ArmAnimSetRow->AimWalkF.ToSoftObjectPath());
					TargetsToStream.Add(ArmAnimSetRow->RunPose.ToSoftObjectPath());
					TargetsToStream.Add(ArmAnimSetRow->JumpOffsetCurveLocation.ToSoftObjectPath());
					TargetsToStream.Add(ArmAnimSetRow->JumpOffsetCurveRotation.ToSoftObjectPath());
					TargetsToStream.Add(ArmAnimSetRow->LandOffsetCurveLocation.ToSoftObjectPath());
					TargetsToStream.Add(ArmAnimSetRow->LandOffsetCurveRotation.ToSoftObjectPath());

					// Montage
					TargetsToStream.Add(ArmAnimSetRow->FireMontage.ToSoftObjectPath());
				}
			}

			// 3P 全身 动画
			if (const FNRBodyAnimSetRow* BodyAnimSetRow = CurrentWeapon->GetWeaponBodyAnimSetRow())
			{
				// Sequence
				TargetsToStream.Add(BodyAnimSetRow->StandIdlePose.ToSoftObjectPath());
				TargetsToStream.Add(BodyAnimSetRow->StandAimPose.ToSoftObjectPath());
				TargetsToStream.Add(BodyAnimSetRow->CrouchIdlePose.ToSoftObjectPath());
				TargetsToStream.Add(BodyAnimSetRow->CrouchAimPose.ToSoftObjectPath());
				TargetsToStream.Add(BodyAnimSetRow->RunPose.ToSoftObjectPath());

				// Montage
				TargetsToStream.Add(BodyAnimSetRow->FireMontage.ToSoftObjectPath());
			}
		}
	}

	UNRStatics::RequestAsyncLoad(StreamableHandle_CurrentWeapon, TargetsToStream);
}
