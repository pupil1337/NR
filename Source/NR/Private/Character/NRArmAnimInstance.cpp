// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRArmAnimInstance.h"

#include "Actor/Weapon/NRWeapon.h"
#include "Character/NRCharacter.h"
#include "Character/Component/NRRunSkiComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void FNRArmAnimInstanceProxy::Initialize(UAnimInstance* InAnimInstance)
{
	FAnimInstanceProxy::Initialize(InAnimInstance);

	// PreView Only
	if (const UNRArmAnimInstance* ArmAnimInstance = Cast<UNRArmAnimInstance>(InAnimInstance))
	{
		AnimSetting = ArmAnimInstance->PreView_AnimSetting;
	}
	
	// Temp
	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		const ANRCharacter* NRCharacterCDO = Cast<ANRCharacter>((NRCharacter->GetClass()->GetDefaultObject()));
		TempMaxWalkSpeed = NRCharacterCDO->GetCharacterMovement()->MaxWalkSpeed;
		TempMaxCrouchSpeed = NRCharacterCDO->GetCharacterMovement()->MaxWalkSpeedCrouched;
		for (const TSubclassOf<UNRComponentBase>& it: NRCharacterCDO->GetAllNRComponentClasses())
		{
			if (const UNRRunSkiComponent* NRRunSkiComponent = Cast<UNRRunSkiComponent>(it->GetDefaultObject()))
			{
				TempMaxRunSpeed = NRRunSkiComponent->GetMaxRunSpeed();
			}
		}
	}
}

void FNRArmAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);

	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		if (NRCharacter->IsLocallyControlled())
		{
			// AnimSetting
			if (ANRWeapon* Weapon = NRCharacter->GetEquippedWeapon())
			{
				if (FNRWeaponInformationRow* WeaponInfo = Weapon->GetWeaponInformation())
				{
					AnimSetting = *WeaponInfo->GetAnimSetting();
				}
			}

			if (const UCharacterMovementComponent* CharacterMovementComponent = NRCharacter->GetCharacterMovement())
			{
				// 1. bCrouching
				bCrouching = CharacterMovementComponent->IsCrouching();
			}

			if (const UNRRunSkiComponent* RunSkiComponent = Cast<UNRRunSkiComponent>(NRCharacter->GetComponentByClass(UNRRunSkiComponent::StaticClass())))
			{
				// 2. bRunning
				bRunning = RunSkiComponent->IsRunning();
			}
			
			const FVector Velocity = UKismetMathLibrary::InverseTransformDirection(NRCharacter->GetActorTransform(), NRCharacter->GetVelocity());
			const FVector VelocityXY = FVector(Velocity.X, Velocity.Y, 0.0f);
			
			const float MaxSpeed = GetCurrMoveModeMaxSpeed();
			// 3. VelocityAlpha
			VelocityAlpha = FMath::Clamp<float>(VelocityXY.Size() / MaxSpeed, 0.0f, 1.0f);
			// 4. VelocityNormalized
			VelocityNormalized = VelocityXY.GetSafeNormal() * VelocityAlpha;
			// 5. VelocityPlayRate
			VelocityPlayRate = VelocityXY.Size() / MaxSpeed;
		}
	}
}

void FNRArmAnimInstanceProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);
}

float FNRArmAnimInstanceProxy::GetCurrMoveModeMaxSpeed() const
{
	if (bCrouching)
	{
		return TempMaxCrouchSpeed;
	}
	if (bRunning)
	{
		return TempMaxRunSpeed;
	}

	return TempMaxWalkSpeed;
}
