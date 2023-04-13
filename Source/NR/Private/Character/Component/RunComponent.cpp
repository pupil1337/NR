// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/RunComponent.h"

#include "Character/NRCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

URunComponent::URunComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void URunComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(URunComponent, bRunning, COND_SkipOwner)
}

void URunComponent::Run(bool NewRun)
{
	// TODO: 检查是否可以run
	
	
	bool OldRunning = bRunning;
	bRunning = NewRun;
	OnRep_Run(OldRunning);

	if (!GetOwner()->HasAuthority())
	{
		Server_Run(NewRun);	
	}
}

void URunComponent::Server_Run_Implementation(bool NewRun)
{
	Run(NewRun);
}

void URunComponent::OnRep_Run(bool OldRunning)
{
	if (ANRCharacter* NRCharacter = Cast<ANRCharacter>(GetOwner()))
	{
		if (UCharacterMovementComponent* CharacterMovementComponent = NRCharacter->GetCharacterMovement())
		{
			if (bRunning)
			{
				CharacterMovementComponent->MaxWalkSpeed = MaxRunSpeed;	
			}
			else
			{
				CharacterMovementComponent->MaxWalkSpeed = Cast<ANRCharacter>(NRCharacter->GetClass()->GetDefaultObject())->GetCharacterMovement()->MaxWalkSpeed;
			}
		}
	}
}

