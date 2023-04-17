// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/NRRunSkiComponent.h"

#include "Character/NRCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

UNRRunSkiComponent::UNRRunSkiComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UNRRunSkiComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UNRRunSkiComponent, bRunning, COND_SkipOwner)
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
		if (!bRunning)
		{
			if (const UCharacterMovementComponent* CharacterMovementComponent = NRCharacter->GetCharacterMovement())
			{
				if (CharacterMovementComponent->IsCrouching())
				{
					NRCharacter->UnCrouch(false);
				}
				else
				{
					NRCharacter->Crouch(false);
				}
			}
		}
		else
		{
			// TODO: 如果是bRunning 则滑行
		}
	}
}

bool UNRRunSkiComponent::CheckCanRun() const
{
	if (NRCharacter && !bRunning)
	{
		if (const UCharacterMovementComponent* CharacterMovementComponent = NRCharacter->GetCharacterMovement())
		{
			return !CharacterMovementComponent->IsFalling() && !CharacterMovementComponent->IsCrouching();
		}	
	}
	return false;
}


void UNRRunSkiComponent::Run(bool NewRun)
{
	// 检查是否可奔跑
	if (bRunning == NewRun || NewRun && !CheckCanRun())
	{
		return;
	}
	
	RunImpl(NewRun);
	if (!GetOwner()->HasAuthority())
	{
		Server_Run(NewRun);
	}
}

void UNRRunSkiComponent::RunImpl(bool NewRun)
{
	if (NRCharacter)
	{
		if (UCharacterMovementComponent* CharacterMovementComponent = NRCharacter->GetCharacterMovement())
		{
			if (NewRun)
			{
				CharacterMovementComponent->MaxWalkSpeed = MaxRunSpeed;
				CharacterMovementComponent->bOrientRotationToMovement = true;
				NRCharacter->bUseControllerRotationYaw = false;
			}
			else
			{
				CharacterMovementComponent->MaxWalkSpeed = Cast<ANRCharacter>(NRCharacter->GetClass()->GetDefaultObject())->GetCharacterMovement()->MaxWalkSpeed;
				CharacterMovementComponent->bOrientRotationToMovement = false;
				NRCharacter->bUseControllerRotationYaw = true;
			}
			
			// bRunning
			bRunning = NewRun;
		}
	}
}

void UNRRunSkiComponent::Server_Run_Implementation(bool NewRun)
{
	Run(NewRun);
}

