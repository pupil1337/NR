// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRBodyAnimInstance.h"

#include "Character/NRCharacter.h"
//#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void FNRBodyAnimInstanceProxy::Initialize(UAnimInstance* InAnimInstance)
{
	FAnimInstanceProxy::Initialize(InAnimInstance);
}

void FNRBodyAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);

	if (ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		FVector Velocity = UKismetMathLibrary::InverseTransformDirection(NRCharacter->GetActorTransform(), NRCharacter->GetVelocity());
		FVector VelocityXY = FVector(Velocity.X, Velocity.Y, 0.0f);
		// float MaxSpeed = Cast<ANRCharacter>(NRCharacter->GetClass()->GetDefaultObject())->GetCharacterMovement()->MaxWalkSpeed; // TODO:蹲伏等

		// 1. MoveDirAlpha
		// 2. MoveDir
		CalculateMoveDirAndAlpha(VelocityXY.GetSafeNormal(), DeltaSeconds);
		
		// 3. bMoving
		bMoving = VelocityXY.X != 0.0f && VelocityXY.Y != 0.0f;
	}
}

void FNRBodyAnimInstanceProxy::Update(float DeltaSeconds)
{
	FAnimInstanceProxy::Update(DeltaSeconds);
}


void FNRBodyAnimInstanceProxy::CalculateMoveDirAndAlpha(const FVector& V, float DeltaSeconds)
{
	// 1. MoveDirAlpha
	MoveDirAlpha.Move_F = FMath::FInterpTo<float>(MoveDirAlpha.Move_F, FMath::Clamp<float>(V.X, 0.0f, 1.0f), DeltaSeconds, 10.0f);
	MoveDirAlpha.Move_B = FMath::FInterpTo<float>(MoveDirAlpha.Move_B, FMath::Abs<float>(FMath::Clamp<float>(V.X, -1.0f, 0.0f)), DeltaSeconds, 10.0f);
	MoveDirAlpha.Move_L = FMath::FInterpTo<float>(MoveDirAlpha.Move_L, FMath::Abs<float>(FMath::Clamp<float>(V.Y, -1.0f, 0.0f)), DeltaSeconds, 10.0f);
	MoveDirAlpha.Move_R = FMath::FInterpTo<float>(MoveDirAlpha.Move_R,  FMath::Clamp<float>(V.Y, 0.0f, 1.0f), DeltaSeconds, 10.0f);

	// 2. MoveDir
	float Angle = UKismetMathLibrary::NormalizedDeltaRotator(V.Rotation(), FRotator::ZeroRotator).Yaw;
	if (-70.0f <= Angle && Angle <= 70.0f)
	{
		MoveDir = ENRMoveDir::EMD_Forward;
	}
	else if (70.0f < Angle && Angle < 110.0f)
	{
		MoveDir = ENRMoveDir::EMD_Right;
	}
	else if (Angle <= -110.0f || Angle >= 110.0f)
	{
		MoveDir = ENRMoveDir::EMD_Backward;
	}
	else
	{
		MoveDir = ENRMoveDir::EMD_Left;
	}
}
