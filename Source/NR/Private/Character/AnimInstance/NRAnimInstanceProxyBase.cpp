// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AnimInstance/NRAnimInstanceProxyBase.h"

#include "Character/NRCharacter.h"
#include "Character/NRCharacterMovementComponent.h"
#include "Character/Component/NRCombatComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Static/NRStatics.h"


FNRAnimInstanceProxyBase::~FNRAnimInstanceProxyBase()
{
	if (StreamableHandlePair.Value.IsValid())
	{
		StreamableHandlePair.Value.Get()->ReleaseHandle();
		StreamableHandlePair.Value.Reset();
	}
}

void FNRAnimInstanceProxyBase::Initialize(UAnimInstance* InAnimInstance)
{
	Super::Initialize(InAnimInstance);

	// Temp
	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		const ANRCharacter* NRCharacterCDO = NRCharacter->GetClass()->GetDefaultObject<ANRCharacter>();
		MaxWalkSpeed = NRCharacterCDO->GetCharacterMovement<UNRCharacterMovementComponent>()->MaxWalkSpeed;
		MaxCrouchSpeed = NRCharacterCDO->GetCharacterMovement<UNRCharacterMovementComponent>()->MaxWalkSpeedCrouched;
		MaxRunSpeed = NRCharacterCDO->GetCharacterMovement<UNRCharacterMovementComponent>()->Run_MaxWalkSpeed;
	}
	else // 不是NRCharacter则当前是动画Instance编辑页面
	{
		LoadAsset(nullptr, true);
	}
}

void FNRAnimInstanceProxyBase::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	Super::PreUpdate(InAnimInstance, DeltaSeconds);

	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(InAnimInstance->TryGetPawnOwner()))
	{
		if (const UNRCharacterMovementComponent* NRCharacterMovementComponent = NRCharacter->GetCharacterMovement<UNRCharacterMovementComponent>())
		{
			// bCrouching
			bCrouching = NRCharacterMovementComponent->IsCrouching();
			// bJumping
			bJumping = NRCharacterMovementComponent->IsFalling();
			// bRunning
			bRunning = NRCharacterMovementComponent->IsRunning();
			// bSkiing
			bSkiing = NRCharacterMovementComponent->IsSkiing();
		}

		const FVector Velocity = UKismetMathLibrary::InverseTransformDirection(NRCharacter->GetActorTransform(), NRCharacter->GetVelocity());
		const FVector VelocityXY = FVector(Velocity.X, Velocity.Y, 0.0f);
		
		// VelocityAlpha
		VelocityAlpha = bJumping || bSkiing ? 0.0f : VelocityXY.Size() / GetCurrMoveModeMaxSpeed();
		
		if (const UNRCombatComponent* CombatComponent = Cast<UNRCombatComponent>(NRCharacter->GetComponentByClass(UNRCombatComponent::StaticClass())))
		{
			// bAiming
			bAiming = CombatComponent->GetIsAiming();
		}
	}
}

void FNRAnimInstanceProxyBase::Update(float DeltaSeconds)
{
	Super::Update(DeltaSeconds);
}

void FNRAnimInstanceProxyBase::LoadAsset(const ANRWeaponBase* WeaponEquipped, bool bForce)
{
	// 当前装备的武器不是已经缓存的资源
	if (StreamableHandlePair.Key != WeaponEquipped || bForce)
	{
		TArray<FSoftObjectPath> TargetsToStream;
		AddSoftObjectPathToArray(TargetsToStream);

		StreamableHandlePair.Key = WeaponEquipped;
		UNRStatics::RequestAsyncLoad(StreamableHandlePair.Value, TargetsToStream);
	}
}

float FNRAnimInstanceProxyBase::GetCurrMoveModeMaxSpeed() const
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
