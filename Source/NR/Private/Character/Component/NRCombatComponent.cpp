// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/NRCombatComponent.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "NRGameSingleton.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/NRCharacter.h"
#include "Character/Component/NRBagComponent.h"
#include "Engine/StreamableManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Static/NRStatics.h"


UNRCombatComponent::UNRCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UNRCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UNRCombatComponent, bAiming, COND_SkipOwner)
}

void UNRCombatComponent::InitLocallyControlledInputEvent(UInputComponent* PlayerInputComponent)
{
	Super::InitLocallyControlledInputEvent(PlayerInputComponent);

	if (NRCharacter)
	{
		if (const APlayerController* PlayerController = NRCharacter->GetController<APlayerController>())
		{
			if (const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
			{
				if (UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
				{
					if (IMC_Combat)
					{
						EnhancedInputLocalPlayerSubsystem->AddMappingContext(IMC_Combat, 0);
						if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
						{
							if (IA_Fire)
							{
								EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Started, this, &ThisClass::StartFireInput);
								EnhancedInputComponent->BindAction(IA_Fire, ETriggerEvent::Completed, this, &ThisClass::EndFireInput);
							}
							if (IA_Aim)
							{
								EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Started, this, &ThisClass::StartAimInput);
								EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Completed, this, &ThisClass::EndAimInput);
							}
						}
					}
				}
			}
		}
	}
}

void UNRCombatComponent::SetEquippedWeapon(ANRWeaponBase* InWeapon)
{
	EquippedWeapon = InWeapon;
	
	// 删除旧的资源
	if (StreamableHandle)
	{
		StreamableHandle.Get()->ReleaseHandle();
		StreamableHandle.Reset();
	}

	if (NRCharacter && EquippedWeapon)
	{
		// 缓存动画蒙太奇资源
		if (const UDataTable* DT_Montage = EquippedWeapon->GetWeaponMontageDT())
		{
			TArray<FSoftObjectPath> AssetsToLoad;
			
			TArray<FNRMontageRow*> MontageRows;
			DT_Montage->GetAllRows<FNRMontageRow>(TEXT("FNRMontageRow"), MontageRows);
			for (const FNRMontageRow* it: MontageRows)
			{
				if (it)
				{
					UNRStatics::AddSoftObjectPathToArray(it->TPS, AssetsToLoad);// 加载TP动画
					if (NRCharacter->IsLocallyControlled())
					{
						UNRStatics::AddSoftObjectPathToArray(it->FPS, AssetsToLoad); // 本地控制端加载FP动画
					}
				}
			}

			StreamableHandle = UNRGameSingleton::Get()->StreamableManager.RequestAsyncLoad(AssetsToLoad);
		}
	}
}

void UNRCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bFireTicking)
	{
		FireTicking(DeltaTime);
	}
}

bool UNRCombatComponent::IsRatePassed(uint32 FireRate) const
{
	return PreShootTime + 60.0f / FireRate <= UGameplayStatics::GetTimeSeconds(this);
}

void UNRCombatComponent::PlayMontageByName(bool bFPS, const FName& RowName) const
{
	if (EquippedWeapon)
	{
		if (UAnimMontage* Montage = EquippedWeapon->GetWeaponMontage(bFPS, RowName))
		{
			if (NRCharacter)
			{
				if (const USkeletalMeshComponent* MeshToPlay = bFPS ? NRCharacter->GetMeshArm() : NRCharacter->GetMesh())
				{
					if (UAnimInstance* AnimInstance = MeshToPlay->GetAnimInstance())
					{
						AnimInstance->Montage_Play(Montage);
					}
				}
			}
		}
	}
}

void UNRCombatComponent::Server_PlayBodyMontageByName_Implementation(const FName& RowName) const
{
	Multi_PlayerBodyMontageByName(RowName);
}

void UNRCombatComponent::Multi_PlayerBodyMontageByName_Implementation(const FName& RowName) const
{
	if (NRCharacter && !NRCharacter->IsLocallyControlled())
	{
		PlayMontageByName(false, RowName);
	}
}

void UNRCombatComponent::OnFire()
{
	PreShootTime = UGameplayStatics::GetTimeSeconds(this);
	// 播放动画
	PlayMontageByName(true, "Fire");
	PlayMontageByName(false, "Fire");
	Server_PlayBodyMontageByName("Fire");
}

void UNRCombatComponent::FireTicking(float DeltaTime)
{
	if (EquippedWeapon)
	{
		if (IsRatePassed(EquippedWeapon->GetWeaponSettingRow()->FireRate))
        {
        	if (EquippedWeapon->GetWeaponSettingRow()->FireMode == ENRWeaponFireMode::EWFM_Automatic)
        	{
        		if (bHoldingFireKey)
        		{
        			OnFire();
        		}
        	}
        }
	}
	
}

void UNRCombatComponent::StartFireInput()
{
	bHoldingFireKey = true;
	if (EquippedWeapon)
	{
		if (EquippedWeapon->GetWeaponSettingRow()->FireMode == ENRWeaponFireMode::EWFM_Automatic)
		{
			bFireTicking = true;
			FireTicking(0.0f);
		}
	}
}

void UNRCombatComponent::EndFireInput()
{
	bHoldingFireKey = false;
}

void UNRCombatComponent::StartAimInput()
{
	// TODO
	OnAim(true);
	if (NRCharacter && NRCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_OnAim(true);
	}
}

void UNRCombatComponent::EndAimInput()
{
	// TODO
	OnAim(false);
	if (NRCharacter && NRCharacter->GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_OnAim(false);
	}
}

void UNRCombatComponent::OnAim(bool bAim)
{
	// TODO
	bAiming = bAim;
}

void UNRCombatComponent::Server_OnAim_Implementation(bool bAim)
{
	// TODO
	OnAim(bAim);
}
