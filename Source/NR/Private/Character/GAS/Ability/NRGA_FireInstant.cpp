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
#include "Table/Weapon/NRAnimSetting.h"
#include "Table/Weapon/NRWeaponSetting.h"
#include "Types/NRCollisionTypes.h"

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
					TA_LineTrace->ConfigParams(100000.0f, NRCollisionProfile::Projectile_ProfileName);
					// TA_LineTrace->bDebug = true; TODO GM
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
					EffectSpecHandle.Data.Get()->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), EquippedWeapon->GetWeaponSettingRow()->InstantDamage);
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
					ASC->ExecuteGameplayCue(FGameplayTag::RequestGameplayTag(FName("GameplayCue.Weapon.Fire"/* UNRGC_Fire */)), CueParams);
					
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
