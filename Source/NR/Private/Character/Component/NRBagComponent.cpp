// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/NRBagComponent.h"

#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/NRCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Character/Component/NRCombatComponent.h"
#include "Manager/NRItemFactory.h"
#include "Net/UnrealNetwork.h"

const FName NAME_Socket_Weapon(TEXT("SOCKET_Weapon"));
const FName NAME_HandR_IkHandGun(TEXT("VB HandR_IkHandGun"));

UNRBagComponent::UNRBagComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UNRBagComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UNRBagComponent, Inventory, COND_None)
	DOREPLIFETIME_CONDITION(UNRBagComponent, CurrentWeapon, COND_SimulatedOnly)
}

void UNRBagComponent::PawnClientRestart()
{
	Super::PawnClientRestart();

	Server_InitInventory();
}

void UNRBagComponent::InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent)
{
	Super::InitLocallyControlledInputEvent(PlayerInputComponent);

	if (NRCharacter)
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

void UNRBagComponent::Server_InitInventory_Implementation()
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

bool UNRBagComponent::Server_InitInventory_Validata()
{
	return true;
}

void UNRBagComponent::AddWeaponToInventory(ANRWeaponBase* NewWeapon, bool bDoEquip/* =false */)
{
	if (NRCharacter && NRCharacter->HasAuthority())
	{
		if (IsWeaponExistInInventory(NewWeapon))
		{
			return;
		}

		Inventory.Weapons.Add(NewWeapon);

		if (bDoEquip)
		{
			EquipWeapon(NewWeapon);
		}
	}
}

void UNRBagComponent::OnRep_Inventory()
{
	
}

void UNRBagComponent::EquipWeapon(ANRWeaponBase* NewWeapon)
{
	if (NRCharacter)
	{
		if (NRCharacter->GetLocalRole() < ROLE_Authority)
		{
			Server_EquipWeapon(NewWeapon);
			SetCurrentWeapon(NewWeapon);
		}
		else
		{
			SetCurrentWeapon(NewWeapon);
		}
	}
}

void UNRBagComponent::Server_EquipWeapon_Implementation(ANRWeaponBase* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

bool UNRBagComponent::Server_EquipWeapon_Validata(ANRWeaponBase* NewWeapon)
{
	return true;
}

void UNRBagComponent::SetCurrentWeapon(ANRWeaponBase* WeaponToEquip)
{
	if (WeaponToEquip == CurrentWeapon)
	{
		return;
	}

	UnEquipWeapon(CurrentWeapon);

	if (WeaponToEquip)
	{
		CurrentWeapon = WeaponToEquip;
		
	}
	
	if (ANRWeaponBase* NewWeapon = UNRItemFactory::SpawnWeapon(this, WeaponType))
	{
		// 销毁旧的武器
		UnEquipWeapon(CurrentWeapon);

		// 设置新武器
		FPSWeapon = Weapon;
		FPSWeapon->SetOwner(NRCharacter);
		FPSWeapon->SetReplicates(false); // 房主生成本地控制的fps武器不同步
		FPSWeapon->SetWeaponState(ENRWeaponState::EWS_Equip);
		FPSWeapon->AttachToComponent(NRCharacter->GetMeshArm(), FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_Socket_Weapon);
		FPSWeapon->SetSelfShadowOnly(true);
		FPSWeapon->SetFPS_SeparateFOV(true, true);
			
		if (UNRCombatComponent* CombatComponent = NRCharacter->GetComponentByClass<UNRCombatComponent>())
		{
			CombatComponent->SetEquippedWeapon(FPSWeapon);
		}
			
		// 通知服务器生成3p武器
		Server_EquipTPSWeapon(WeaponType);
	}
}

void UNRBagComponent::UnEquipWeapon(ANRWeaponBase* WeaponToUnEquip)
{
}

void UNRBagComponent::OnRep_CurrentWeapon(ANRWeaponBase* OldWeapon)
{
	if (OldWeapon)
	{
		OldWeapon->Destroy();
	}

	if (NRCharacter && TPSWeapon)
	{
		if (NRCharacter->IsLocallyControlled())
		{
			TPSWeapon->SetOnlySeeShadow(true);
		}
		else
		{
			if (UNRCombatComponent* CombatComponent = NRCharacter->GetComponentByClass<UNRCombatComponent>())
			{
				CombatComponent->SetEquippedWeapon(TPSWeapon);
			}
		}
	}
}

void UNRBagComponent::TryEquipWeaponInSlot(uint8 Slot)
{
	if (Slot < 1 || Slot > 4)
	{
		return;
	}
}

// Utils ==============================================================================
bool UNRBagComponent::IsWeaponExistInInventory(ANRWeaponBase* InWeapon)
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
