// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRArmAnimInstance.h"

#include "NRGameSingleton.h"
#include "NRStatics.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/NRCharacter.h"
#include "Character/NRCharacterMovementComponent.h"
#include "Character/Component/NRBagComponent.h"
#include "Kismet/KismetMathLibrary.h"

const FName NAME_Track_Location(TEXT("Location"));
const FName NAME_Track_Rotation(TEXT("Rotation"));

FNRArmAnimInstanceProxy::~FNRArmAnimInstanceProxy()
{
	if (StreamableHandlePair.Value)
	{
		StreamableHandlePair.Value.Get()->ReleaseHandle();
		StreamableHandlePair.Value.Reset();
	}
}

void FNRArmAnimInstanceProxy::Initialize(UAnimInstance* InAnimInstance)
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
	LoadAsset(nullptr, true);
#endif
}

void FNRArmAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);

	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		if (NRCharacter->IsLocallyControlled())
		{
			// AnimSetting
			if (const UNRBagComponent* BagComponent = Cast<UNRBagComponent>(NRCharacter->GetComponentByClass(UNRBagComponent::StaticClass())))
			{
				if (ANRWeaponBase* Weapon = BagComponent->GetFPSWeapon())
				{
					if (const FNRWeaponInformationRow* WeaponInfo = Weapon->GetWeaponInformation())
					{
						AnimSetting = *WeaponInfo->GetArmAnimSet();
						LoadAsset(Weapon);
					}
				}
			}

			if (const UNRCharacterMovementComponent* NRCharacterMovementComponent = NRCharacter->GetCharacterMovement<UNRCharacterMovementComponent>())
			{
				// 1. bCrouching
				bCrouching = NRCharacterMovementComponent->IsCrouching();
				// 2. bJumping
				bJumping = NRCharacterMovementComponent->IsFalling();
				// 3. bRunning
				bRunning = NRCharacter->bRunning;
				// 4. bSkiing
				bSkiing = NRCharacter->bSkiing;
			}
			
			const FVector Velocity = UKismetMathLibrary::InverseTransformDirection(NRCharacter->GetActorTransform(), NRCharacter->GetVelocity());
			const FVector VelocityXY = FVector(Velocity.X, Velocity.Y, 0.0f);
			
			const float MaxSpeed = GetCurrMoveModeMaxSpeed();
			// 5. VelocityAlpha
			VelocityAlpha = FMath::Clamp<float>(VelocityXY.Size() / MaxSpeed, 0.0f, 1.0f);
			// 6. VelocityNormalized
			VelocityNormalized = VelocityXY.GetSafeNormal() * VelocityAlpha;
			// 7. VelocityPlayRate
			VelocityPlayRate = bJumping || bSkiing ? 0.0f : VelocityXY.Size() / MaxSpeed;

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
			// 8. JumpOffset_Location
			JumpOffset_Location += CurveLocation->GetVectorValue(InTime);
		}
		if (CurveRotation)
		{
			// 9. JumpOffset_Rotation
			const FVector RotationV = CurveRotation->GetVectorValue(InTime);
			JumpOffset_Rotation += FRotator(RotationV.Y, RotationV.Z, RotationV.X);
		}
	};
	if (bPlayJump)
	{
		JumpSeconds += DeltaSeconds;
		ApplyJumpOffset(AnimSetting.JumpOffsetCurveLocation.Get(), AnimSetting.JumpOffsetCurveRotation.Get(), JumpSeconds);
	}
	if (bPlayLand)
	{
		LandSeconds += DeltaSeconds;
		ApplyJumpOffset(AnimSetting.LandOffsetCurveLocation.Get(), AnimSetting.LandOffsetCurveRotation.Get(), LandSeconds);
	}
}

void FNRArmAnimInstanceProxy::LoadAsset(const ANRWeaponBase* WeaponEquipped, bool bForce /* = false */)
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
			UNRStatics::AddSoftObjectPathToArray(AnimSetting.IdlePose, AssetsToLoad);
			UNRStatics::AddSoftObjectPathToArray(AnimSetting.BreathingStandPose, AssetsToLoad);
			UNRStatics::AddSoftObjectPathToArray(AnimSetting.RunPose, AssetsToLoad);
			UNRStatics::AddSoftObjectPathToArray(AnimSetting.JumpOffsetCurveLocation, AssetsToLoad);
			UNRStatics::AddSoftObjectPathToArray(AnimSetting.JumpOffsetCurveRotation, AssetsToLoad);
			UNRStatics::AddSoftObjectPathToArray(AnimSetting.LandOffsetCurveLocation, AssetsToLoad);
			UNRStatics::AddSoftObjectPathToArray(AnimSetting.LandOffsetCurveRotation, AssetsToLoad);

			StreamableHandlePair.Key = WeaponEquipped;
			StreamableHandlePair.Value = NRGameSingleton->StreamableManager.RequestAsyncLoad(AssetsToLoad);
		}
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
