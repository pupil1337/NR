// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRBodyAnimInstance.h"

#include "Character/NRCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

const FName NAME_Curve_Feet_Crossing(TEXT("Feet_Crossing"));

void FNRBodyAnimInstanceProxy::Initialize(UAnimInstance* InAnimInstance)
{
	FAnimInstanceProxy::Initialize(InAnimInstance);
}

void FNRBodyAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);

	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		FVector Velocity = UKismetMathLibrary::InverseTransformDirection(NRCharacter->GetActorTransform(), NRCharacter->GetVelocity());
		FVector VelocityXY = FVector(Velocity.X, Velocity.Y, 0.0f);
		FVector VelocityXY_Normalized = VelocityXY.GetSafeNormal();
		// float MaxSpeed = Cast<ANRCharacter>(NRCharacter->GetClass()->GetDefaultObject())->GetCharacterMovement()->MaxWalkSpeed; // TODO:蹲伏等
		float MoveAngle = UKismetMathLibrary::NormalizedDeltaRotator(VelocityXY_Normalized.Rotation(), FRotator::ZeroRotator).Yaw;
		
		// 1. MoveDirAlpha
		// 2. MoveDir
		CalculateMoveDirAndAlpha(VelocityXY_Normalized, MoveAngle, DeltaSeconds);
		
		// 3. bMoving
		bMoving = VelocityXY.X != 0.0f && VelocityXY.Y != 0.0f;

		if (const UCharacterMovementComponent* CharacterMovementComponent = NRCharacter->GetCharacterMovement())
		{
			// 4. bJumping
			bJumping = CharacterMovementComponent->IsFalling();
            
			// 5. bCrouching
			bCrouching = CharacterMovementComponent->IsCrouching();
		}

		// 6. AO_Yaw
		// 7. AO_Pitch
		// 8. TurnDir
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
	
	// 6. AO_Yaw
	if (!bMoving && !bJumping) // 静止
	{
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrAimRotation, StartAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;

		// 8. TurnDir
		if (AO_Yaw < -70.0f) TurnDir.SetTurnL();
		else if (AO_Yaw > 70.0f) TurnDir.SetTurnR();

		if (TurnDir.None == false)
		{
			// 在转身
			AO_Yaw = InterpAO_Yaw = FMath::FInterpTo<float>(InterpAO_Yaw, 0.0f, DeltaSeconds, 15.0f);
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

	// 7. AO_Pitch
	AO_Pitch = BaseAimRotation.Pitch;
	if (!bLocallyControlled && AO_Pitch > 90.0f)
	{
		FVector2D InRange = {270.0f, 360.0f};
		FVector2D OutRange = {-90.0f, 0.0f};
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void FNRBodyAnimInstanceProxy::UpdateCurvesValue(UAnimInstance* InAnimInstance)
{
	Curves.bFeetCrossing = InAnimInstance->GetCurveValue(NAME_Curve_Feet_Crossing) == 1.0f ? true : false;
}

void FNRBodyAnimInstanceProxy::UpdateOtherValues()
{
	AO_Pitch_Negate = -AO_Pitch;
}
