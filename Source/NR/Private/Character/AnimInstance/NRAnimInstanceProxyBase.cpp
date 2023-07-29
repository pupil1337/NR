// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AnimInstance/NRAnimInstanceProxyBase.h"

#include "Character/NRCharacter.h"
#include "Character/NRCharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


FNRAnimInstanceProxyBase::~FNRAnimInstanceProxyBase()
{
}

void FNRAnimInstanceProxyBase::Initialize(UAnimInstance* InAnimInstance)
{
	Super::Initialize(InAnimInstance);

	// Temp
	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		const ANRCharacter* NRCharacterCDO = NRCharacter->GetClass()->GetDefaultObject<ANRCharacter>();
		MaxWalkSpeed = NRCharacterCDO->GetCharacterMovement<UNRCharacterMovementComponent>()->MaxWalkSpeed;
		MaxCrouchSpeed = NRCharacterCDO->GetCharacterMovement<UNRCharacterMovementComponent>()->MaxWalkSpeedCrouched;
		MaxRunSpeed = NRCharacterCDO->GetCharacterMovement<UNRCharacterMovementComponent>()->Run_MaxWalkSpeed;
	}
}

void FNRAnimInstanceProxyBase::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	Super::PreUpdate(InAnimInstance, DeltaSeconds);

	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		if (const UNRCharacterMovementComponent* NRCharacterMovementComponent = NRCharacter->GetCharacterMovement<UNRCharacterMovementComponent>())
		{
			// bCrouching
			bCrouching = NRCharacterMovementComponent->IsCrouching();
			// bJumping
			bJumping = NRCharacterMovementComponent->IsFalling();
			// bRunning
			bRunning = NRCharacterMovementComponent->IsRunning();
			// bSkiing
			bSkiing = NRCharacterMovementComponent->IsSkiing();
		}

		const FVector Velocity = UKismetMathLibrary::InverseTransformDirection(NRCharacter->GetActorTransform(), NRCharacter->GetVelocity());
		const FVector VelocityXY = FVector(Velocity.X, Velocity.Y, 0.0f);
		
		// VelocityAlpha
		VelocityAlpha = bJumping || bSkiing ? 0.0f : VelocityXY.Size() / GetCurrMoveModeMaxSpeed();

		// TODO
		// if (const UNRCombatComponent* CombatComponent = Cast<UNRCombatComponent>(NRCharacter->GetComponentByClass(UNRCombatComponent::StaticClass())))
		// {
		// 	// bAiming
		// 	bAiming = CombatComponent->GetIsAiming();
		// }
	}
}

void FNRAnimInstanceProxyBase::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);
}

float FNRAnimInstanceProxyBase::GetCurrMoveModeMaxSpeed() const
{
	if (bCrouching)
	{
		return MaxCrouchSpeed;
	}
	if (bRunning)
	{
		return MaxRunSpeed;
	}

	return MaxWalkSpeed;
}
