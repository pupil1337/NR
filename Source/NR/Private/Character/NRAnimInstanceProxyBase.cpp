// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NRAnimInstanceProxyBase.h"

#include "NRGameSingleton.h"
#include "Character/NRCharacter.h"
#include "Character/NRCharacterMovementComponent.h"
#include "Character/Component/NRCombatComponent.h"
#include "Kismet/KismetMathLibrary.h"


FNRAnimInstanceProxyBase::~FNRAnimInstanceProxyBase()
{
	if (StreamableHandlePair.Value)
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

#ifdef WITH_EDITORONLY_DATA
	LoadAsset(nullptr, true);
#endif
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
		VelocityAlpha = VelocityXY.Size() / GetCurrMoveModeMaxSpeed();
		
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
			AddSoftObjectPathToArray(AssetsToLoad);

			StreamableHandlePair.Key = WeaponEquipped;
			StreamableHandlePair.Value = NRGameSingleton->StreamableManager.RequestAsyncLoad(AssetsToLoad);
		}
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
