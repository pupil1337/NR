// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRArmAnimInstance.h"

#include "Character/NRCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void FNRArmAnimInstanceProxy::Initialize(UAnimInstance* InAnimInstance)
{
	FAnimInstanceProxy::Initialize(InAnimInstance);
}

void FNRArmAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);

	if (ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		if (NRCharacter->IsLocallyControlled())
		{
			FVector Velocity = UKismetMathLibrary::InverseTransformDirection(NRCharacter->GetActorTransform(), NRCharacter->GetVelocity());
			FVector VelocityXY = FVector(Velocity.X, Velocity.Y, 0.0f);
			float MaxSpeed = Cast<ANRCharacter>(NRCharacter->GetClass()->GetDefaultObject())->GetCharacterMovement()->MaxWalkSpeed; // TODO:蹲伏等
			// 1. VelocityAlpha
			VelocityAlpha = FMath::Clamp<float>(VelocityXY.Size() / MaxSpeed, 0.0f, 1.0f);
			// 2. VelocityNormalized
			VelocityNormalized = VelocityXY.GetSafeNormal() * VelocityAlpha;
			// 3. VelocityPlayRate
			VelocityPlayRate = FMath::Max<float>(VelocityXY.Size() / MaxSpeed, 1.0f);
			// 4. BreathingAlpha
			BreathingAlpha = 1.0f - VelocityAlpha;
		}
	}
}

void FNRArmAnimInstanceProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);
}
