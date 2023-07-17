// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRCharacterMovementComponent.h"

#include "Character/NRCharacter.h"
#include "Components/CapsuleComponent.h"
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

void UNRCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	NRCharacterOwner = Cast<ANRCharacter>(GetOwner());
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
	
	if (NRCharacterOwner && NRCharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		// Run
		if (MovementMode == MOVE_Walking)
		{
			MaxWalkSpeed = bWantsToRun ? Run_MaxWalkSpeed : NRCharacterOwner->GetClass()->GetDefaultObject<ANRCharacter>()->GetCharacterMovement<UNRCharacterMovementComponent>()->MaxWalkSpeed;
		}
		NRCharacterOwner->bRunning = bWantsToRun;
	}
}

void UNRCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	if (NRCharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)
	{
		// Ski
		if (MovementMode == MOVE_Walking && bWantsToRun && bWantsToCrouch)
		{
			FHitResult PotentialSkiSurface;
			if (Velocity.SizeSquared() > pow(MaxWalkSpeedCrouched, 2) && GetSkiSurface(PotentialSkiSurface))
			{
				EnterSki(PotentialSkiSurface);
			}
		}
		if (IsNRMovementMode(NRMOVE_Ski) && !bWantsToCrouch)
		{
			ExitSki(false);
		}
	}
	
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UNRCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case NRMOVE_Ski:
		PhySki(deltaTime, Iterations);
		break;
	default:
		UE_LOG(LogTemp, Fatal, TEXT("错误的NR自定义移动模式"))
	}
}

bool UNRCharacterMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsNRMovementMode(NRMOVE_Ski);
}

bool UNRCharacterMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

bool UNRCharacterMovementComponent::CanAttemptJump() const
{
	return IsJumpAllowed() &&
		   // !bWantsToCrouch &&
		   (IsMovingOnGround() || IsFalling()); // Falling included for double-jump and non-zero jump hold time, but validated by character.
}

bool UNRCharacterMovementComponent::IsNRMovementMode(ENRMovementMode InNRMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InNRMovementMode;
}

// Run
void UNRCharacterMovementComponent::Run(bool bRun)
{
	bWantsToRun = bRun;
	if (bWantsToRun)
	{
		bWantsToCrouch = false;
	}
}

// Ski
void UNRCharacterMovementComponent::EnterSki(const FHitResult& PotentialSkiSurface)
{
	bWantsToRun = false;
	NRCharacterOwner->bSkiing = true;

	const FVector VelPlaneDir = FVector::VectorPlaneProject(Velocity, PotentialSkiSurface.Normal).GetSafeNormal();
	Velocity += VelPlaneDir * Ski_EnterImpulse;
	SetMovementMode(MOVE_Custom, NRMOVE_Ski);
}

void UNRCharacterMovementComponent::PhySki(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	RestorePreAdditiveRootMotionVelocity();

	FHitResult SurfaceHit;
	if (!GetSkiSurface(SurfaceHit))
	{
		ExitSki(false);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}
	if (Velocity.SizeSquared() < pow(MaxWalkSpeedCrouched, 2))
	{
		ExitSki(true);
		StartNewPhysics(deltaTime, Iterations);
		return;
	}

	// 重力
	Velocity += FMath::Abs(GetGravityZ()) * FVector::DownVector * deltaTime;

	FVector VelPlaneDir = FVector::VectorPlaneProject(Velocity, SurfaceHit.Normal).GetSafeNormal(); // 速度沿着平面的投影
	if (VelPlaneDir.Rotation().Pitch <= Ski_MinSlopeToHasAcceleration)
	{
		// 坡度较大时额外加速度
		Velocity += Ski_AddAcceleration * deltaTime * VelPlaneDir;
	}

	// TODO: 允许玩家微控方向
	Acceleration = FVector::ZeroVector;

	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(deltaTime, Ski_Friction, true, GetMaxBrakingDeceleration());
	}
	ApplyRootMotionToVelocity(deltaTime);

	// Perform Move
	++Iterations;
	bJustTeleported = false;

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
	FHitResult Hit(1.0f);
	FVector Adjusted = Velocity * deltaTime; // 位移
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(FVector::VectorPlaneProject(UpdatedComponent->GetForwardVector(), SurfaceHit.Normal).GetSafeNormal(), SurfaceHit.Normal).ToQuat();
	SafeMoveUpdatedComponent(Adjusted, NewRotation, true, Hit);

	if (Hit.Time < 1.0f)
	{
		HandleImpact(Hit, deltaTime, Adjusted);
		// TODO: 参考Walking 编写Setup
		SlideAlongSurface(Adjusted, (1.0f - Hit.Time), Hit.Normal, Hit, true);
	}

	FHitResult NewSurfaceHit;
	if (!GetSkiSurface(NewSurfaceHit))
	{
		ExitSki(false);
	}
	else if (Velocity.SizeSquared() < pow(MaxWalkSpeedCrouched, 2))
	{
		ExitSki(true);
	}

	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}
}

void UNRCharacterMovementComponent::ExitSki(bool bKeepCrouch)
{
	bWantsToCrouch = bKeepCrouch;
	NRCharacterOwner->bSkiing = false;
	const FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(), FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, Hit);
	SetMovementMode(bKeepCrouch ? MOVE_Walking : MOVE_Falling);
}

bool UNRCharacterMovementComponent::GetSkiSurface(FHitResult& Hit) const
{
	FCollisionQueryParams Params;
	TArray<AActor*> NRCharacterChildren;
	NRCharacterOwner->GetAllChildActors(NRCharacterChildren);
	Params.AddIgnoredActors(NRCharacterChildren);
	Params.AddIgnoredActor(NRCharacterOwner);
	return GetWorld()->LineTraceSingleByProfile(
		Hit,
		UpdatedComponent->GetComponentLocation(),
		UpdatedComponent->GetComponentLocation() + NRCharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.0f * FVector::DownVector,
		UCollisionProfile::BlockAll_ProfileName,
		Params
	);
}
