// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Ability/NRGA_FireInstant.h"

#include "AbilitySystemGlobals.h"
#include "Actor/Weapon/NRWeaponBase.h"
#include "Character/NRCharacter.h"
#include "Character/Component/NRInventoryComponent.h"
#include "Character/GAS/TargetActor/NRGATA_LineTrace.h"
#include "Character/GAS/Task/NRAT_ServerWaitClientTargetData.h"
#include "Character/GAS/Task/NRAT_WaitTargetDataUsingActor.h"

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
			UE_LOG(LogTemp, Log, TEXT("FireInstant:: FireBullet(LocallyControlled)"))

			if (EquippedWeapon)
			{
				UNRAT_WaitTargetDataUsingActor* AT_WaitTargetDataUsingActor = UNRAT_WaitTargetDataUsingActor::WaitTargetDataUsingActor(this, EGameplayTargetingConfirmation::Type::Instant, EquippedWeapon->GetLineTraceTargetActor(), true);
				AT_WaitTargetDataUsingActor->ValidData.AddDynamic(this, &ThisClass::HandleTargetData);
				AT_WaitTargetDataUsingActor->ReadyForActivation();
			}	
		}
	}
}

void UNRGA_FireInstant::HandleTargetData(const FGameplayAbilityTargetDataHandle& Data)
{
	if (GetCurrentActorInfo())
	{
		UE_LOG(LogTemp, Log, TEXT("FireInstant:: HandleTargetData [%s, Role:%d]"), GetCurrentActorInfo()->IsNetAuthority() ? TEXT("Server") : TEXT("Client"), GetOwningActorFromActorInfo()->GetLocalRole())
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
