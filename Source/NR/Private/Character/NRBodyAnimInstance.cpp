﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRBodyAnimInstance.h"

#include "Actor/Weapon/NRWeapon.h"
#include "Character/NRCharacter.h"
#include "Character/Component/NRRunSkiComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Table/Weapon/NRWeaponInformation.h"

const FName NAME_Curve_Feet_Crossing(TEXT("Feet_Crossing"));

void FNRBodyAnimInstanceProxy::Initialize(UAnimInstance* InAnimInstance)
{
	FAnimInstanceProxy::Initialize(InAnimInstance);

	// PreView Only
#if WITH_EDITORONLY_DATA
	if (const UNRBodyAnimInstance* ArmAnimInstance = Cast<UNRBodyAnimInstance>(InAnimInstance))
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

void FNRBodyAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);

	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		// AnimSetting
		if (const ANRWeapon* Weapon = NRCharacter->GetEquippedWeapon())
		{
			if (const FNRWeaponInformationRow* WeaponInfo = Weapon->GetWeaponInformation())
			{
				AnimSetting = *WeaponInfo->GetAnimSetting();
			}
		}
		
		const FVector Velocity = UKismetMathLibrary::InverseTransformDirection(NRCharacter->GetActorTransform(), NRCharacter->GetVelocity());
		const FVector VelocityXY = FVector(Velocity.X, Velocity.Y, 0.0f);
		const FVector VelocityXY_Normalized = VelocityXY.GetSafeNormal();
		const float MoveAngle = UKismetMathLibrary::NormalizedDeltaRotator(VelocityXY_Normalized.Rotation(), FRotator::ZeroRotator).Yaw;
		
		// 1. MoveDirAlpha
		// 2. MoveDir
		CalculateMoveDirAndAlpha(VelocityXY_Normalized, MoveAngle, DeltaSeconds);

		// 3. VelocityPlayRate
		VelocityPlayRate = VelocityXY.Size() / GetCurrMoveModeMaxSpeed();
		
		// 4. bMoving
		bMoving = VelocityXY.X != 0.0f && VelocityXY.Y != 0.0f;

		if (const UCharacterMovementComponent* CharacterMovementComponent = NRCharacter->GetCharacterMovement())
		{
			// 5. bJumping
			bJumping = CharacterMovementComponent->IsFalling();
            
			// 6. bCrouching
			bCrouching = CharacterMovementComponent->IsCrouching();
		}

		if (const UNRRunSkiComponent* NRRunSkiComponent = Cast<UNRRunSkiComponent>(NRCharacter->GetComponentByClass(UNRRunSkiComponent::StaticClass())))
		{
			// 7. bRunning
			bRunning = NRRunSkiComponent->IsRunning();
		}

		// 8.  AO_Yaw
		// 9.  AO_Pitch
		// 10. TurnDir
		UpdateAimOffset(NRCharacter->GetBaseAimRotation(), NRCharacter->IsLocallyControlled(), DeltaSeconds);
	}

	// Update Curves
	UpdateCurvesValue(InAnimInstance);

	// Update Other
	UpdateOtherValues();
}

void FNRBodyAnimInstanceProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);
}


void FNRBodyAnimInstanceProxy::CalculateMoveDirAndAlpha(const FVector& V, float MoveAngle, float DeltaSeconds)
{
	// 1. MoveDirAlpha
	MoveDirAlpha.Move_F = FMath::FInterpTo<float>(MoveDirAlpha.Move_F, FMath::Clamp<float>(V.X, 0.0f, 1.0f), DeltaSeconds, 10.0f);
	MoveDirAlpha.Move_B = FMath::FInterpTo<float>(MoveDirAlpha.Move_B, FMath::Abs<float>(FMath::Clamp<float>(V.X, -1.0f, 0.0f)), DeltaSeconds, 10.0f);
	MoveDirAlpha.Move_L = FMath::FInterpTo<float>(MoveDirAlpha.Move_L, FMath::Abs<float>(FMath::Clamp<float>(V.Y, -1.0f, 0.0f)), DeltaSeconds, 10.0f);
	MoveDirAlpha.Move_R = FMath::FInterpTo<float>(MoveDirAlpha.Move_R,  FMath::Clamp<float>(V.Y, 0.0f, 1.0f), DeltaSeconds, 10.0f);

	// 2. MoveDir
	if (-70.0f <= MoveAngle && MoveAngle <= 70.0f) MoveDir.SetMoveF();
	else if (70.0f < MoveAngle && MoveAngle < 110.0f) MoveDir.SetMoveR();
	else if (MoveAngle <= -110.0f || MoveAngle >= 110.0f) MoveDir.SetMoveB();
	else MoveDir.SetMoveL();
}

void FNRBodyAnimInstanceProxy::UpdateAimOffset(const FRotator& BaseAimRotation, bool bLocallyControlled, float DeltaSeconds)
{
	FRotator CurrAimRotation = FRotator(0.0f, BaseAimRotation.Yaw, 0.0f);

	auto InitAimOffset = [this, CurrAimRotation]() -> void
	{
		StartAimRotation = CurrAimRotation;
		AO_Yaw = 0.0f;
		TurnDir.SetTurnNone();
	};
	
	// 8. AO_Yaw
	if (!bMoving && !bJumping) // 静止
	{
		const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrAimRotation, StartAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;

		// 10. TurnDir
		if (AO_Yaw < -70.0f) TurnDir.SetTurnL();
		else if (AO_Yaw > 70.0f) TurnDir.SetTurnR();

		if (TurnDir.None == false)
		{
			// 在转身
			AO_Yaw = InterpAO_Yaw = FMath::FInterpTo<float>(InterpAO_Yaw, 0.0f, DeltaSeconds, 5.0f);
			if (FMath::Abs(AO_Yaw) < 5.0f)
			{
				InitAimOffset();
			}
		}
		else
		{
			// 不在转身
			InterpAO_Yaw = AO_Yaw;
		}
	}
	else // 移动
	{
		InitAimOffset();
	}

	// 9. AO_Pitch
	AO_Pitch = BaseAimRotation.Pitch;
	if (!bLocallyControlled && AO_Pitch > 90.0f)
	{
		const FVector2D InRange = {270.0f, 360.0f};
		const FVector2D OutRange = {-90.0f, 0.0f};
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

float FNRBodyAnimInstanceProxy::GetCurrMoveModeMaxSpeed() const
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

void FNRBodyAnimInstanceProxy::UpdateCurvesValue(const UAnimInstance* InAnimInstance)
{
	Curves.bFeetCrossing = InAnimInstance->GetCurveValue(NAME_Curve_Feet_Crossing) == 0.0f ? false : true;
	// GEngine->AddOnScreenDebugMessage(1, 1.0f, FColor::Green, FString::Printf(TEXT("FeetCrossing CurveValue %f"), InAnimInstance->GetCurveValue(NAME_Curve_Feet_Crossing)));
}

void FNRBodyAnimInstanceProxy::UpdateOtherValues()
{
	AO_Pitch_Negate = -AO_Pitch;
	bCrouchingAndMoving = bCrouching && bMoving;
	bNotCrouchingAndMoving = !bCrouching && bMoving;
}
