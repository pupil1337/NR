// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRCharacterMovementComponent.h"

#include "Character/NRCharacter.h"
#include "GameFramework/Character.h"

class ANRCharacter;

// FSavedMove_NR =======================================================================================================
FSavedMove_NR::FSavedMove_NR()
	: bWantsToRun(0)
{
}

bool FSavedMove_NR::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
	const FSavedMove_NR* NewNRMove = static_cast<FSavedMove_NR*>(NewMove.Get());
	if (bWantsToRun != NewNRMove->bWantsToRun)
	{
		return false;
	}
	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void FSavedMove_NR::Clear()
{
	Super::Clear();

	bWantsToRun = 0;
}

uint8 FSavedMove_NR::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();
	if (bWantsToRun)
	{
		Result |= FLAG_Custom_0;
	}
	
	return Result;
}

void FSavedMove_NR::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	if (const UNRCharacterMovementComponent* NRCharacterMovementComponent = C->GetCharacterMovement<UNRCharacterMovementComponent>())
	{
		bWantsToRun = NRCharacterMovementComponent->bWantsToRun;
	}
}

void FSavedMove_NR::PrepMoveFor(ACharacter* C)
{
	Super::PrepMoveFor(C);

	if (UNRCharacterMovementComponent* NRCharacterMovementComponent = C->GetCharacterMovement<UNRCharacterMovementComponent>())
	{
		NRCharacterMovementComponent->bWantsToRun = bWantsToRun;
	}
}

// FNetworkPredictionData_Client_NR ====================================================================================
FNetworkPredictionData_Client_NR::FNetworkPredictionData_Client_NR(const UCharacterMovementComponent& ClientMovement)
	: Super(ClientMovement)
{
}

FSavedMovePtr FNetworkPredictionData_Client_NR::AllocateNewMove()
{
	return FSavedMovePtr(new FSavedMove_NR());
}

// UNRCharacterMovementComponent =======================================================================================
UNRCharacterMovementComponent::UNRCharacterMovementComponent()
{
}

FNetworkPredictionData_Client* UNRCharacterMovementComponent::GetPredictionData_Client() const
{
	if (ClientPredictionData == nullptr)
	{
		UNRCharacterMovementComponent* MutableThis = const_cast<UNRCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_NR(*this);
	}
	return ClientPredictionData;
}


void UNRCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bWantsToRun = (Flags & FSavedMove_NR::FLAG_Custom_0) != 0;
}

void UNRCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	if (ANRCharacter* NRCharacter = Cast<ANRCharacter>(CharacterOwner))
	{
		if (MovementMode == MOVE_Walking)
		{
			// Run
			if (bWantsToRun)
			{
				MaxWalkSpeed = Run_MaxWalkSpeed;
			}
			else
			{
				MaxWalkSpeed = NRCharacter->GetClass()->GetDefaultObject<ANRCharacter>()->GetCharacterMovement<UNRCharacterMovementComponent>()->MaxWalkSpeed;
			}
			if (NRCharacter->GetLocalRole() != ROLE_SimulatedProxy)
			{
				NRCharacter->bRunning = bWantsToRun;
			}
		}	
	}
}
