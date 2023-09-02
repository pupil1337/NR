// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_FireInstant.h"

#include "AbilitySystemGlobals.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/NRCharacter.h"
#include "Character/Component/NRInventoryComponent.h"
#include "Character/GAS/TargetActor/NRTA_LineTrace.h"
#include "Character/GAS/Task/NRAT_PlayMontageForMeshAndWait.h"
#include "Character/GAS/Task/NRAT_ServerWaitClientTargetData.h"
#include "Character/GAS/Task/NRAT_WaitTargetDataUsingActor.h"
#include "PlayerController/NRPlayerController.h"
#include "Table/Weapon/NRAnimSetting.h"
#include "Table/Weapon/NRWeaponSetting.h"
#include "Types/NRCollisionTypes.h"

#ifdef IMGUI_API
#include "DebugConsole/NRDebugConsole.h"
#endif

UNRGA_FireInstant::UNRGA_FireInstant()
{
	
}

bool UNRGA_FireInstant::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}


bool UNRGA_FireInstant::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UNRGA_FireInstant::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ResetData(ActorInfo))
	{
		UNRAT_ServerWaitClientTargetData* AT_ServerWaitClientTargetData = UNRAT_ServerWaitClientTargetData::ServerWaitClientTargetData(this, false);
		AT_ServerWaitClientTargetData->ValidData.AddDynamic(this, &ThisClass::HandleTargetData);
		AT_ServerWaitClientTargetData->ReadyForActivation();
	
		FireBullet();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);	
	}
}

void UNRGA_FireInstant::FireBullet()
{
	if (GetCurrentActorInfo() && GetCurrentActorInfo()->IsLocallyControlled())
	{
		if (UAbilitySystemGlobals::Get().ShouldIgnoreCosts() || CheckCost(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo()))
		{
			// UE_LOG(LogTemp, Log, TEXT("FireInstant:: FireBullet(LocallyControlled)"))

			if (EquippedWeapon)
			{
				if (ANRTA_LineTrace* TA_LineTrace = EquippedWeapon->GetLineTraceTargetActor())
				{
					FGameplayAbilityTargetingLocationInfo StartLocation;
					StartLocation.LocationType = EGameplayAbilityTargetingLocationType::SocketTransform;
					StartLocation.SourceComponent = EquippedWeapon->GetMesh1P();
					StartLocation.SourceSocketName = NAME_Socket_Muzzle;
					TA_LineTrace->ConfigParams(StartLocation, 100000.0f, NRCollisionProfile::Projectile_ProfileName);
#ifdef IMGUI_API
					if (const FGameplayAbilityActorInfo* ActorInfo = GetCurrentActorInfo())
					{
						if (const ANRPlayerController* NRPlayerController = Cast<ANRPlayerController>(ActorInfo->PlayerController))
						{
							if (NRPlayerController->DebugConsole.IsValid())
							{
								TA_LineTrace->bDebug = NRPlayerController->DebugConsole->FireTraceDebug;
							}
						}
					}
#endif
					UNRAT_WaitTargetDataUsingActor* AT_WaitTargetDataUsingActor = UNRAT_WaitTargetDataUsingActor::WaitTargetDataUsingActor(this, EGameplayTargetingConfirmation::Type::Instant, TA_LineTrace, true);
					AT_WaitTargetDataUsingActor->ValidData.AddDynamic(this, &ThisClass::HandleTargetData);
					AT_WaitTargetDataUsingActor->ReadyForActivation();	
				}
			}
		}
	}
}

void UNRGA_FireInstant::HandleTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle)
{
	// TODO

	// 动画
	PlayFireMontage();

	// GE + GC
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		switch (EquippedWeapon->GetWeaponSettingRow()->BulletType)
		{
			case ENRBulletType::EBT_Instant:
				{
					// TODO 子弹穿透
					// Gameplay Effect
					const FGameplayEffectSpecHandle& EffectSpecHandle = MakeOutgoingGameplayEffectSpec(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), BulletInstantEffectClass, 1.0f);
					EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(NRGameplayTag::Data_Damage, EquippedWeapon->GetWeaponSettingRow()->InstantDamage);
					// ReSharper disable once CppExpressionWithoutSideEffects
					ApplyGameplayEffectSpecToTarget(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), EffectSpecHandle, TargetDataHandle);

					// Gameplay Cue
					FGameplayEffectContextHandle EffectContextHandle = EffectSpecHandle.Data.Get()->GetEffectContext();
					if (TargetDataHandle.Data.IsValidIndex(0))
					{
						if (const FGameplayAbilityTargetData* Data = TargetDataHandle.Data[0].Get())
						{
							if (Data->HasHitResult())
							{
								// TODO 散弹枪
								EffectContextHandle.AddHitResult(*Data->GetHitResult());
							}
						}
					}

					FGameplayCueParameters CueParams;
					CueParams.EffectContext = EffectContextHandle;
					CueParams.SourceObject = EquippedWeapon;
					ASC->ExecuteGameplayCue(NRGameplayTag::GC_Weapon_Fire/* UNRGC_Fire */, CueParams);
					
					break;	
				}
			case ENRBulletType::EBT_Projectile:
				// TODO ProjectileEffectClass
				break;
			
			default: ;
		}
	}
}

void UNRGA_FireInstant::PlayFireMontage()
{
	if (GetCurrentActorInfo() && EquippedWeapon)
	{
		if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(GetCurrentActorInfo()->AvatarActor))
		{
			// UE_LOG(LogTemp, Log, TEXT("FireInstant:: HandleTargetData [%s, Role:%d]"), GetCurrentActorInfo()->IsNetAuthority() ? TEXT("Server") : TEXT("Client"), GetOwningActorFromActorInfo()->GetLocalRole())

			if (NRCharacter->IsLocallyControlled())
			{
				if (const FNRArmAnimSetRow* ArmAnimSetRow = EquippedWeapon->GetWeaponArmAnimSetRow())
				{
					UNRAT_PlayMontageForMeshAndWait* AT_PlayMontageForMeshAndWait_FPS = UNRAT_PlayMontageForMeshAndWait::PlayMontageForMeshAndWait(this, NRCharacter->GetMeshArm(), ArmAnimSetRow->FireMontage.Get());
					AT_PlayMontageForMeshAndWait_FPS->ReadyForActivation();
				}	
			}

			if (const FNRBodyAnimSetRow* BodyAnimSetRow = EquippedWeapon->GetWeaponBodyAnimSetRow())
			{
				UNRAT_PlayMontageForMeshAndWait* AT_PlayMontageForMeshAndWait_TPS = UNRAT_PlayMontageForMeshAndWait::PlayMontageForMeshAndWait(this, NRCharacter->GetMesh(), BodyAnimSetRow->FireMontage.Get());
				AT_PlayMontageForMeshAndWait_TPS->ReadyForActivation();	
			}
		}
	}
}

// Utils ==============================================================================================
ANRWeaponBase* UNRGA_FireInstant::GetEquippedWeaponFromActorInfo(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (const UNRInventoryComponent* InventoryComponent = NRCharacter->GetInventoryComponent())
		{
			return InventoryComponent->GetCurrentWeapon();
		}
	}
	
	return nullptr;
}

bool UNRGA_FireInstant::ResetData(const FGameplayAbilityActorInfo* ActorInfo)
{
	EquippedWeapon = GetEquippedWeaponFromActorInfo(ActorInfo);
	if (EquippedWeapon)
	{
		// TODO
		return true;
	}
	
	return false;
}
