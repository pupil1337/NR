// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/NRCombatComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/NRCharacter.h"
#include "Net/UnrealNetwork.h"


UNRCombatComponent::UNRCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
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

void UNRCombatComponent::StartFireInput()
{
}

void UNRCombatComponent::EndFireInput()
{
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


