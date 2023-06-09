// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/NRCombatComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/NRCharacter.h"


UNRCombatComponent::UNRCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
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
}

void UNRCombatComponent::EndAimInput()
{
}

