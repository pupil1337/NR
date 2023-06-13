// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRArmAnimInstance.h"

#include "NRStatics.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/NRCharacter.h"
#include "Character/Component/NRBagComponent.h"
#include "Kismet/KismetMathLibrary.h"

const FName NAME_Track_Location(TEXT("Location"));
const FName NAME_Track_Rotation(TEXT("Rotation"));

void FNRArmAnimInstanceProxy::Initialize(UAnimInstance* InAnimInstance)
{
	Super::Initialize(InAnimInstance);
}

void FNRArmAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	Super::PreUpdate(InAnimInstance, DeltaSeconds);

	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		if (NRCharacter->IsLocallyControlled())
		{
			/** AnimSetting */
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
			
			const FVector Velocity = UKismetMathLibrary::InverseTransformDirection(NRCharacter->GetActorTransform(), NRCharacter->GetVelocity());
			const FVector VelocityXY = FVector(Velocity.X, Velocity.Y, 0.0f);
			/** VelocityNormalized */
			VelocityNormalized = VelocityXY.GetSafeNormal() * VelocityAlpha;

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
	Super::Update(DeltaSeconds);

	// Jump Offset
	JumpOffset_Location = FVector::ZeroVector;
	JumpOffset_Rotation = FRotator::ZeroRotator;
	auto ApplyJumpOffset = [this](const UCurveVector* CurveLocation, const UCurveVector* CurveRotation, float InTime)-> void
	{
		if (CurveLocation)
		{
			/** JumpOffset_Location */
			JumpOffset_Location += CurveLocation->GetVectorValue(InTime);
		}
		if (CurveRotation)
		{
			/** JumpOffset_Rotation */
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

void FNRArmAnimInstanceProxy::AddSoftObjectPathToArray(TArray<FSoftObjectPath>& AssetsToLoad)
{
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.IdlePose, AssetsToLoad);
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.IdleBreath, AssetsToLoad);
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.AimPose, AssetsToLoad);
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.AimBreath, AssetsToLoad);
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.AimWalkF, AssetsToLoad);
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.RunPose, AssetsToLoad);
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.JumpOffsetCurveLocation, AssetsToLoad);
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.JumpOffsetCurveRotation, AssetsToLoad);
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.LandOffsetCurveLocation, AssetsToLoad);
	UNRStatics::AddSoftObjectPathToArray(AnimSetting.LandOffsetCurveRotation, AssetsToLoad);
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
