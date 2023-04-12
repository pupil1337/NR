// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRArmAnimInstance.h"

#include "Actor/Weapon/NRWeapon.h"
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
			// 1. AnimSetting
			if (ANRWeapon* Weapon = NRCharacter->GetEquippedWeapon())
			{
				AnimSetting = Weapon->GetAnimSetting();
			}
			
			FVector Velocity = UKismetMathLibrary::InverseTransformDirection(NRCharacter->GetActorTransform(), NRCharacter->GetVelocity());
			FVector VelocityXY = FVector(Velocity.X, Velocity.Y, 0.0f);
			float MaxSpeed = Cast<ANRCharacter>(NRCharacter->GetClass()->GetDefaultObject())->GetCharacterMovement()->MaxWalkSpeed;
			// 2. VelocityAlpha
			VelocityAlpha = FMath::Clamp<float>(VelocityXY.Size() / MaxSpeed, 0.0f, 1.0f);
			// 3. VelocityNormalized
			VelocityNormalized = VelocityXY.GetSafeNormal() * VelocityAlpha;
			// 4. VelocityPlayRate
			VelocityPlayRate = VelocityXY.Size() / MaxSpeed;

			if (const UCharacterMovementComponent* CharacterMovementComponent = NRCharacter->GetCharacterMovement())
			{
				// 5. bCrouching
				bCrouching = CharacterMovementComponent->IsCrouching();
			}
		}
	}
}

void FNRArmAnimInstanceProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);
}
