// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRArmAnimInstance.h"

#include "Actor/Weapon/NRWeapon.h"
#include "Character/NRCharacter.h"
#include "Character/Component/NRRunSkiComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

const FName NAME_Track_Location(TEXT("Location"));
const FName NAME_Track_Rotation(TEXT("Rotation"));

void FNRArmAnimInstanceProxy::Initialize(UAnimInstance* InAnimInstance)
{
	FAnimInstanceProxy::Initialize(InAnimInstance);

#if WITH_EDITORONLY_DATA
	// PreView Only
	if (const UNRArmAnimInstance* ArmAnimInstance = Cast<UNRArmAnimInstance>(InAnimInstance))
	{
		AnimSetting = ArmAnimInstance->PreView_AnimSetting;
	}
#endif
	
	// Temp
	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		const ANRCharacter* NRCharacterCDO = Cast<ANRCharacter>((NRCharacter->GetClass()->GetDefaultObject()));
		MaxWalkSpeed = NRCharacterCDO->GetCharacterMovement()->MaxWalkSpeed;
		MaxCrouchSpeed = NRCharacterCDO->GetCharacterMovement()->MaxWalkSpeedCrouched;
		for (const TSubclassOf<UNRComponentBase>& it: NRCharacterCDO->GetAllNRComponentClasses())
		{
			if (const UNRRunSkiComponent* NRRunSkiComponent = Cast<UNRRunSkiComponent>(it->GetDefaultObject()))
			{
				MaxRunSpeed = NRRunSkiComponent->GetMaxRunSpeed();
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
				if (const FNRWeaponInformationRow* WeaponInfo = Weapon->GetWeaponInformation())
				{
					AnimSetting = *WeaponInfo->GetAnimSetting();
				}
			}

			if (const UCharacterMovementComponent* CharacterMovementComponent = NRCharacter->GetCharacterMovement())
			{
				// 1. bCrouching
				bCrouching = CharacterMovementComponent->IsCrouching();
				// 2. bJumping
				bJumping = CharacterMovementComponent->IsFalling();
			}

			if (const UNRRunSkiComponent* RunSkiComponent = Cast<UNRRunSkiComponent>(NRCharacter->GetComponentByClass(UNRRunSkiComponent::StaticClass())))
			{
				// 3. bRunning
				bRunning = RunSkiComponent->IsRunning();
			}
			
			const FVector Velocity = UKismetMathLibrary::InverseTransformDirection(NRCharacter->GetActorTransform(), NRCharacter->GetVelocity());
			const FVector VelocityXY = FVector(Velocity.X, Velocity.Y, 0.0f);
			
			const float MaxSpeed = GetCurrMoveModeMaxSpeed();
			// 4. VelocityAlpha
			VelocityAlpha = FMath::Clamp<float>(VelocityXY.Size() / MaxSpeed, 0.0f, 1.0f);
			// 5. VelocityNormalized
			VelocityNormalized = VelocityXY.GetSafeNormal() * VelocityAlpha;
			// 6. VelocityPlayRate
			VelocityPlayRate = bJumping ? 0.0f : VelocityXY.Size() / MaxSpeed;

			// Jump Offset
			if (const UNRArmAnimInstance* NRArmAnimInstance = Cast<UNRArmAnimInstance>(InAnimInstance))
			{
				if (LandStamp != NRArmAnimInstance->LandStamp)
				{
					LandStamp = NRArmAnimInstance->LandStamp;
					LandSeconds = 0.0f;
					bPlayLand = true;
					bPlayJump = false;
				}
				else if (JumpStamp != NRArmAnimInstance->JumpStamp)
				{
					JumpStamp = NRArmAnimInstance->JumpStamp;
					JumpSeconds = 0.0f;
					bPlayJump = true;
					bPlayLand = false;
				}
			}
		}
	}
}

void FNRArmAnimInstanceProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);

	// Jump Offset
	JumpOffset_Location = FVector::ZeroVector;
	JumpOffset_Rotation = FRotator::ZeroRotator;
	auto ApplyJumpOffset = [this](const UCurveVector* CurveLocation, const UCurveVector* CurveRotation, float InTime)-> void
	{
		if (CurveLocation)
		{
			// 7. JumpOffset_Location
			JumpOffset_Location += CurveLocation->GetVectorValue(InTime);
		}
		if (CurveRotation)
		{
			// 8. JumpOffset_Rotation
			const FVector RotationV = CurveRotation->GetVectorValue(InTime);
			JumpOffset_Rotation += FRotator(RotationV.Y, RotationV.Z, RotationV.X);
		}
	};
	if (bPlayJump)
	{
		JumpSeconds += DeltaSeconds;
		ApplyJumpOffset(AnimSetting.JumpOffsetCurveLocation, AnimSetting.JumpOffsetCurveRotation, JumpSeconds);
	}
	if (bPlayLand)
	{
		LandSeconds += DeltaSeconds;
		ApplyJumpOffset(AnimSetting.LandOffsetCurveLocation, AnimSetting.LandOffsetCurveRotation, LandSeconds);
	}
}

float FNRArmAnimInstanceProxy::GetCurrMoveModeMaxSpeed() const
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

// UNRArmAnimInstance===================================================================================================
void UNRArmAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (ANRCharacter* NRCharacter = Cast<ANRCharacter>(TryGetPawnOwner()))
	{
		NRCharacter->OnJumpedEvent.AddUniqueDynamic(this, &ThisClass::OnJumped);
		NRCharacter->LandedDelegate.AddUniqueDynamic(this, &ThisClass::OnLanded);
	}
}

void UNRArmAnimInstance::OnJumped()
{
	JumpStamp += 1;
}

void UNRArmAnimInstance::OnLanded(const FHitResult& Hit)
{
	LandStamp += 1;
}
