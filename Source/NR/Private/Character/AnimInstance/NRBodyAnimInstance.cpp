// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AnimInstance/NRBodyAnimInstance.h"

#include "Static/NRStatics.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/NRCharacter.h"
#include "Character/Component/NRBagComponent.h"
#include "Engine/StreamableManager.h"
#include "Kismet/KismetMathLibrary.h"

const FName NAME_Curve_Feet_Crossing(TEXT("Feet_Crossing"));

FNRBodyAnimInstanceProxy::~FNRBodyAnimInstanceProxy()
{
	if (StreamableHandlePair.Value)
	{
		StreamableHandlePair.Value.Get()->ReleaseHandle();
		StreamableHandlePair.Value.Reset();
	}
}

void FNRBodyAnimInstanceProxy::Initialize(UAnimInstance* InAnimInstance)
{
	Super::Initialize(InAnimInstance);
}

void FNRBodyAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	Super::PreUpdate(InAnimInstance, DeltaSeconds);

	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		/** AnimSetting */
		if (const UNRBagComponent* BagComponent = Cast<UNRBagComponent>(NRCharacter->GetComponentByClass(UNRBagComponent::StaticClass())))
		{
			if (ANRWeaponBase* Weapon = BagComponent->GetTPSWeapon())
			{
				if (Weapon != StreamableHandlePair.Key)
				{
					AnimSetting = *Weapon->GetWeaponBodyAnimSetRow();
					LoadAsset(Weapon);
				}
			}
		}
		
		// MoveDirAlpha | MoveDir
		const FVector Velocity = UKismetMathLibrary::InverseTransformDirection(NRCharacter->GetActorTransform(), NRCharacter->GetVelocity());
		const FVector VelocityXY = FVector(Velocity.X, Velocity.Y, 0.0f);
		const FVector VelocityXY_Normalized = VelocityXY.GetSafeNormal();
		const float MoveAngle = UKismetMathLibrary::NormalizedDeltaRotator(VelocityXY_Normalized.Rotation(), FRotator::ZeroRotator).Yaw;
		CalculateMoveDirAndAlpha(VelocityXY_Normalized, MoveAngle, DeltaSeconds);
		
		/** bMoving */
		bMoving = VelocityXY.X != 0.0f && VelocityXY.Y != 0.0f;
		
		// AO_Yaw | AO_Pitch | TurnDir
		UpdateAimOffset(NRCharacter->GetBaseAimRotation(), NRCharacter->IsLocallyControlled(), DeltaSeconds);
	}

	// Update Curves
	UpdateCurvesValue(InAnimInstance);

	// Update Other
	UpdateOtherValues();
}

void FNRBodyAnimInstanceProxy::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);
}

void FNRBodyAnimInstanceProxy::AddSoftObjectPathToArray(TArray<FSoftObjectPath>& OutTargetsToStream)
{
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.StandIdlePose, OutTargetsToStream);
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.StandAimPose, OutTargetsToStream);
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.CrouchIdlePose, OutTargetsToStream);
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.CrouchAimPose, OutTargetsToStream);
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.RunPose, OutTargetsToStream);
}

void FNRBodyAnimInstanceProxy::CalculateMoveDirAndAlpha(const FVector& V, float MoveAngle, float DeltaSeconds)
{
	/** MoveDirAlpha */
	MoveDirAlpha.Move_F = FMath::FInterpTo<float>(MoveDirAlpha.Move_F, FMath::Clamp<float>(V.X, 0.0f, 1.0f), DeltaSeconds, 10.0f);
	MoveDirAlpha.Move_B = FMath::FInterpTo<float>(MoveDirAlpha.Move_B, FMath::Abs<float>(FMath::Clamp<float>(V.X, -1.0f, 0.0f)), DeltaSeconds, 10.0f);
	MoveDirAlpha.Move_L = FMath::FInterpTo<float>(MoveDirAlpha.Move_L, FMath::Abs<float>(FMath::Clamp<float>(V.Y, -1.0f, 0.0f)), DeltaSeconds, 10.0f);
	MoveDirAlpha.Move_R = FMath::FInterpTo<float>(MoveDirAlpha.Move_R,  FMath::Clamp<float>(V.Y, 0.0f, 1.0f), DeltaSeconds, 10.0f);

	/** MoveDir */
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
	
	/** AO_Yaw */
	if (!bMoving && !bJumping) // 静止
	{
		const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrAimRotation, StartAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;

		/** TurnDir */
		if (AO_Yaw < -70.0f) TurnDir.SetTurnL();
		else if (AO_Yaw > 70.0f) TurnDir.SetTurnR();

		if (TurnDir.Left || TurnDir.Right)
		{
			// 在转身
			AO_Yaw = InterpAO_Yaw = FMath::FInterpTo<float>(InterpAO_Yaw, 0.0f, DeltaSeconds, 10.0f);
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

	/** AO_Pitch */
	AO_Pitch = BaseAimRotation.Pitch;
	if (!bLocallyControlled && AO_Pitch > 90.0f)
	{
		const FVector2D InRange = {270.0f, 360.0f};
		const FVector2D OutRange = {-90.0f, 0.0f};
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void FNRBodyAnimInstanceProxy::UpdateCurvesValue(const UAnimInstance* InAnimInstance)
{
	Curves.bFeetCrossing = InAnimInstance->GetCurveValue(NAME_Curve_Feet_Crossing) == 0.0f ? false : true;
}

void FNRBodyAnimInstanceProxy::UpdateOtherValues()
{
	AO_Pitch_Negate = -AO_Pitch;
	bCrouchingAndMoving = bCrouching && bMoving;
	bNotCrouchingAndMoving = !bCrouching && bMoving;
}
