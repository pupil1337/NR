// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRBodyAnimInstance.h"

#include "NRGameSingleton.h"
#include "NRStatics.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/NRCharacter.h"
#include "Character/NRCharacterMovementComponent.h"
#include "Character/Component/NRBagComponent.h"
#include "Character/Component/NRCombatComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Table/Weapon/NRWeaponInformation.h"

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
	FAnimInstanceProxy::Initialize(InAnimInstance);

	// Temp
	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		const ANRCharacter* NRCharacterCDO = NRCharacter->GetClass()->GetDefaultObject<ANRCharacter>();
		MaxWalkSpeed = NRCharacterCDO->GetCharacterMovement<UNRCharacterMovementComponent>()->MaxWalkSpeed;
		MaxCrouchSpeed = NRCharacterCDO->GetCharacterMovement<UNRCharacterMovementComponent>()->MaxWalkSpeedCrouched;
		MaxRunSpeed = NRCharacterCDO->GetCharacterMovement<UNRCharacterMovementComponent>()->Run_MaxWalkSpeed;
	}

#ifdef WITH_EDITORONLY_DATA
	this->LoadAsset(nullptr, true);
#endif
}

void FNRBodyAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);

	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		// AnimSetting
		if (const UNRBagComponent* BagComponent = Cast<UNRBagComponent>(NRCharacter->GetComponentByClass(UNRBagComponent::StaticClass())))
		{
			if (ANRWeaponBase* Weapon = BagComponent->GetTPSWeapon())
			{
				if (const FNRWeaponInformationRow* WeaponInfo = Weapon->GetWeaponInformation())
				{
					AnimSetting = *WeaponInfo->GetBodyAnimSet();
					LoadAsset(Weapon);
				}
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

		if (const UNRCharacterMovementComponent* NRCharacterMovementComponent = NRCharacter->GetCharacterMovement<UNRCharacterMovementComponent>())
		{
			// 5. bJumping
			bJumping = NRCharacterMovementComponent->IsFalling();
            
			// 6. bCrouching
			bCrouching = NRCharacterMovementComponent->IsCrouching();

			// 7. bRunning
			bRunning = NRCharacter->bRunning;

			// 9. bSkiing
			bSkiing = NRCharacter->bSkiing;
		}

		if (const UNRCombatComponent* CombatComponent = Cast<UNRCombatComponent>(NRCharacter->GetComponentByClass(UNRCombatComponent::StaticClass())))
		{
			// 8. bAiming
			bAiming = CombatComponent->GetIsAiming();
		}

		// 10.  AO_Yaw
		// 11.  AO_Pitch
		// 12. TurnDir
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

void FNRBodyAnimInstanceProxy::LoadAsset(const ANRWeaponBase* WeaponEquipped, bool bForce /* = false */)
{
	// 如果当前装备的武器不是已经缓存的资源
	if (StreamableHandlePair.Key != WeaponEquipped || bForce)
	{
		// 1.GC旧的武器相关资源
		if (StreamableHandlePair.Value)
		{
			StreamableHandlePair.Value.Get()->ReleaseHandle();
			StreamableHandlePair.Value.Reset();
		}

		// 2.加载当前使用的武器相关资源
		if (UNRGameSingleton* NRGameSingleton = UNRGameSingleton::Get())
		{
			TArray<FSoftObjectPath> AssetsToLoad;
			UNRStatics::AddSoftObjectPathToArray(AnimSetting.StandIdlePose, AssetsToLoad);
			UNRStatics::AddSoftObjectPathToArray(AnimSetting.StandAimPose, AssetsToLoad);
			UNRStatics::AddSoftObjectPathToArray(AnimSetting.CrouchIdlePose, AssetsToLoad);
			UNRStatics::AddSoftObjectPathToArray(AnimSetting.CrouchAimPose, AssetsToLoad);
			UNRStatics::AddSoftObjectPathToArray(AnimSetting.RunPose, AssetsToLoad);

			StreamableHandlePair.Key = WeaponEquipped;
			StreamableHandlePair.Value = NRGameSingleton->StreamableManager.RequestAsyncLoad(AssetsToLoad);
		}
	}
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
	
	// 9. AO_Yaw
	if (!bMoving && !bJumping) // 静止
	{
		const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrAimRotation, StartAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;

		// 11. TurnDir
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

	// 10. AO_Pitch
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
