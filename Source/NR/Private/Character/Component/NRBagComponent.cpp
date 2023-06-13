// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/NRBagComponent.h"

#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/NRCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "NRStatics.h"
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

	DOREPLIFETIME_CONDITION(UNRBagComponent, TPSWeapon, COND_None)
}

void UNRBagComponent::PawnClientRestart()
{
	Super::PawnClientRestart();

	EquipFPSWeapon(TEXT("AR_01"));
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

void UNRBagComponent::EquipFPSWeapon(FName WeaponRowName)
{
	if (NRCharacter)
	{
		if (ANRWeaponBase* Weapon = UNRItemFactory::SpawnWeapon(this, WeaponRowName))
		{
			Weapon->SetOwner(NRCharacter);
			Weapon->SetReplicates(false); // 房主生成本地控制的fps武器不同步
			Weapon->SetWeaponState(ENRWeaponState::EWS_Equip);
			Weapon->AttachToComponent(NRCharacter->GetMeshArm(), FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_Socket_Weapon);
			UNRStatics::SetFPS_SeparateFOV(Weapon->GetMesh(), true, true);

			if (FPSWeapon)
			{
				FPSWeapon->Destroy();
			}
			FPSWeapon = Weapon;
			Server_EquipTPSWeapon(WeaponRowName);
		}
	}
}

void UNRBagComponent::Server_EquipTPSWeapon_Implementation(FName WeaponRowName)
{
	if (NRCharacter)
	{
		if (ANRWeaponBase* Weapon = UNRItemFactory::SpawnWeapon(this, WeaponRowName))
		{
			Weapon->SetOwner(NRCharacter);
			Weapon->SetReplicates(true);
			Weapon->SetWeaponState(ENRWeaponState::EWS_Equip);
			Weapon->AttachToComponent(NRCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, NAME_HandR_IkHandGun);

			ANRWeaponBase* OldWeapon = TPSWeapon;
			TPSWeapon = Weapon;
			OnRep_TPSWeapon(OldWeapon);
		}
	}
}

void UNRBagComponent::OnRep_TPSWeapon(ANRWeaponBase* OldWeapon)
{
	if (OldWeapon)
	{
		OldWeapon->Destroy();
	}

	if (NRCharacter && TPSWeapon && NRCharacter->IsLocallyControlled())
	{
		TPSWeapon->SetRenderInMainPass(false);
	}
}

void UNRBagComponent::TryEquipWeaponInSlot(uint8 Slot)
{
	if (Slot < 1 || Slot > 4)
	{
		return;
	}
}
