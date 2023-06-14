// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/NRCombatComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/NRCharacter.h"
#include "Character/Component/NRBagComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"


UNRCombatComponent::UNRCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UNRCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UNRCombatComponent, bAiming, COND_SkipOwner)
}

void UNRCombatComponent::InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent)
{
	Super::InitLocallyControlledInputEvent(PlayerInputComponent);

	if (NRCharacter)
	{
		if (const APlayerController* PlayerController = NRCharacter->GetController<APlayerController>())
		{
			if (const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					if (IMC_Combat)
					{
						EnhancedInputLocalPlayerSubsystem->AddMappingContext(IMC_Combat, 0);
						if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
						{
							if (IA_Fire)
							{
								EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Started, this, &ThisClass::StartFireInput);
								EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Completed, this, &ThisClass::EndFireInput);
							}
							if (IA_Aim)
							{
								EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Started, this, &ThisClass::StartAimInput);
								EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Completed, this, &ThisClass::EndAimInput);
							}
						}
					}
				}
			}
		}
	}
}

void UNRCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bFireTicking)
	{
		FireTicking(DeltaTime);
	}
}

bool UNRCombatComponent::UpdateWeaponAndWeaponSettingRaw()
{
	if (NRCharacter)
	{
		if (const UNRBagComponent* BagComponent = Cast<UNRBagComponent>(NRCharacter->GetComponentByClass(UNRBagComponent::StaticClass())))
		{
			if (ANRWeaponBase* FPSWeapon = BagComponent->GetFPSWeapon())
			{
				if (Weapon != FPSWeapon)
				{
					if (const FNRWeaponInformationRow* WeaponInformation = FPSWeapon->GetWeaponInformation())
					{
						if (const FNRWeaponSettingRow* WeaponSetting = WeaponInformation->GetWeaponSetting())
						{
							Weapon = FPSWeapon;
							WeaponSettingRaw = *WeaponSetting;
							return true;
						}
					}
				}
				else
				{
					return Weapon != nullptr;
				}
			}
		}	
	}
	return false;
}

bool UNRCombatComponent::IsRatePassed(uint32 FireRate) const
{
	return PreShootTime + 60.0f / FireRate <= UGameplayStatics::GetTimeSeconds(this);
}

void UNRCombatComponent::OnFire()
{
	PreShootTime = UGameplayStatics::GetTimeSeconds(this);
}

void UNRCombatComponent::FireTicking(float DeltaTime)
{
	if (IsRatePassed(WeaponSettingRaw.FireRate))
	{
		if (WeaponSettingRaw.FireMode == ENRWeaponFireMode::EWFM_Automatic)
		{
			if (bHoldingFireKey)
			{
				
			}
		}
	}
}

void UNRCombatComponent::StartFireInput()
{
	bHoldingFireKey = true;
	if (UpdateWeaponAndWeaponSettingRaw())
	{
		if (WeaponSettingRaw.FireMode == ENRWeaponFireMode::EWFM_Automatic)
		{
			bFireTicking = true;
			FireTicking(0.0f);
		}
	}
}

void UNRCombatComponent::EndFireInput()
{
	bHoldingFireKey = false;
}

void UNRCombatComponent::StartAimInput()
{
	// TODO
	OnAim(true);
	if (NRCharacter && NRCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_OnAim(true);
	}
}

void UNRCombatComponent::EndAimInput()
{
	// TODO
	OnAim(false);
	if (NRCharacter && NRCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_OnAim(false);
	}
}

void UNRCombatComponent::OnAim(bool bAim)
{
	// TODO
	bAiming = bAim;
}

void UNRCombatComponent::Server_OnAim_Implementation(bool bAim)
{
	// TODO
	OnAim(bAim);
}
