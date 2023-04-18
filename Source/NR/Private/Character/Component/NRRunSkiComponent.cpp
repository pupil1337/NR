// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/NRRunSkiComponent.h"

#include "Character/NRCharacter.h"
#include "Character/NRCharacterMovementComponent.h"

UNRRunSkiComponent::UNRRunSkiComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UNRRunSkiComponent::InitLocallyControlledInputEvent()
{
	Super::InitLocallyControlledInputEvent();

	if (NRCharacter)
	{
		NRCharacter->OnInputEvent_Move.AddUniqueDynamic(this, &ThisClass::OnMoveInput);
		NRCharacter->OnInputEvent_Run.AddUniqueDynamic(this, &ThisClass::OnRunInput);
		NRCharacter->OnInputEvent_Crouch.AddUniqueDynamic(this, &ThisClass::OnCrouchInput);
	}
}

void UNRRunSkiComponent::OnMoveInput(const FInputActionValue& Value)
{
	MoveInputValue = Value.Get<FInputActionValue::Axis2D>();
	if (MoveInputValue.Y != 1.0f)
	{
		Run(false);
	}
}

void UNRRunSkiComponent::OnRunInput()
{
	if (MoveInputValue.Y == 1.0f)
	{
		Run(true);
	}
}

void UNRRunSkiComponent::OnCrouchInput()
{
	if (NRCharacter)
	{
		if (const UNRCharacterMovementComponent* NRCharacterMovementComponent = NRCharacter->GetCharacterMovement<UNRCharacterMovementComponent>())
		{
			if (!NRCharacter->bRunning)
			{
				if (NRCharacterMovementComponent->IsCrouching())
				{
					NRCharacter->UnCrouch(false);
				}
				else
				{
					NRCharacter->Crouch(false);
				}
			}
			else
			{
				// TODO: 如果是bRunning 则滑行
			}
		}
	}
}

bool UNRRunSkiComponent::CheckCanRun() const
{
	if (NRCharacter && !NRCharacter->bRunning)
	{
		if (const UNRCharacterMovementComponent* NRCharacterMovementComponent = NRCharacter->GetCharacterMovement<UNRCharacterMovementComponent>())
		{
			return !NRCharacterMovementComponent->IsFalling();
		}	
	}
	return false;
}


void UNRRunSkiComponent::Run(bool NewRun)
{
	if (NRCharacter)
	{
		if (NRCharacter->bRunning == NewRun || NewRun && !CheckCanRun())
		{
			return;
		}

		if (UNRCharacterMovementComponent* NRCharacterMovementComponent = NRCharacter->GetCharacterMovement<UNRCharacterMovementComponent>())
		{
			NRCharacterMovementComponent->bWantsToRun = NewRun;
			if (NewRun && NRCharacterMovementComponent->IsCrouching())
			{
				NRCharacter->UnCrouch();
			}
		}
	}
}

