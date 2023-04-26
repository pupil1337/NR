﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/NRBagComponent.h"

#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/NRCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Net/UnrealNetwork.h"

const FName NAME_Socket_Weapon(TEXT("SOCKET_Weapon"));
const FName NAME_Socket_VB_SOCKET_hand_r_ik_hand_gun(TEXT("VB SOCKET_hand_r_ik_hand_gun"));

UNRBagComponent::UNRBagComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UNRBagComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UNRBagComponent, EquippedWeapon, COND_None)
	DOREPLIFETIME_CONDITION(UNRBagComponent, WeaponSlot, COND_None)
}

void UNRBagComponent::BeginPlay()
{
	Super::BeginPlay();

	// TODO:换成从存档中读取武器数据,然后生成武器
	if (NRCharacter && NRCharacter->HasAuthority())
	{
		if (WeaponClass)
		{
			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			Params.Owner = NRCharacter;
			WeaponSlot[0] = GetWorld()->SpawnActor<ANRWeaponBase>(WeaponClass, Params);
			EquippedWeapon = WeaponSlot[0];
			OnRep_EquippedWeapon(nullptr);
		}
	}
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

void UNRBagComponent::TryEquipWeaponInSlot(uint8 Slot)
{
	if (Slot < 1 || Slot > 4)
	{
		return;
	}
}

void UNRBagComponent::OnRep_EquippedWeapon(const ANRWeaponBase* OldEquippedWeapon) const
{
	if (NRCharacter)
	{
		// OldEquip
		if (OldEquippedWeapon)
		{
			// ....
		}

		// NewEquip
		if (NRCharacter->IsLocallyControlled())
		{
			EquippedWeapon->AttachToComponent(NRCharacter->GetMeshArm(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_Socket_Weapon);
		}
		else
		{
			EquippedWeapon->AttachToComponent(NRCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, NAME_Socket_VB_SOCKET_hand_r_ik_hand_gun);
		}
	}
}