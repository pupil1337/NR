// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Attribute/NRAS_Character.h"

#include "GameplayEffectExtension.h"
#include "Types/NRGASTypes.h"
#include "Net/UnrealNetwork.h"

UNRAS_Character::UNRAS_Character()
{
}

void UNRAS_Character::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Character, Health, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Character, MaxHealth, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Character, Shield, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Character, MaxShield, COND_None, REPNOTIFY_Always)
}

void UNRAS_Character::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	// TODO
}

void UNRAS_Character::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	check(Data.Target.GetAvatarActor()->GetLocalRole() == ROLE_Authority)
	
	if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		SetShield(FMath::Clamp<float>(GetShield(), 0.0f, GetMaxShield()));
	}
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		float LocalDamageDone = GetDamage();
		SetDamage(0.0f);

		// 应用最近收到伤害Tag
		if (!Data.Target.HasMatchingGameplayTag(NRGameplayTag::State_RecentlyDamaged))
		{
			Data.Target.AddLooseGameplayTag(NRGameplayTag::State_RecentlyDamaged);
		}
		this->GetWorld()->GetTimerManager().ClearTimer(RecentlyDamagedHandle);
		this->GetWorld()->GetTimerManager().SetTimer(RecentlyDamagedHandle, FTimerDelegate::CreateLambda([Data, this]()
			{
				Data.Target.RemoveLooseGameplayTag(NRGameplayTag::State_RecentlyDamaged);
			}),
		5.0f, false);
		
		// 减去护盾值
		if (LocalDamageDone > 0.0f)
		{
			float OldShield = GetShield();
			if (OldShield > 0.0f)
			{
				SetShield(FMath::Clamp<float>(OldShield - LocalDamageDone, 0.0f, GetMaxShield()));
				LocalDamageDone -= OldShield;
			}
		}

		// 减去血量值
		if (LocalDamageDone > 0.0f)
		{
			SetHealth(FMath::Clamp<float>(GetHealth() - LocalDamageDone, 0.0f, GetMaxHealth()));
		}

		// TODO
	}
}

void UNRAS_Character::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Character, Health, OldHealth)
}

void UNRAS_Character::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Character, MaxHealth, OldMaxHealth)
}

void UNRAS_Character::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Character, Shield, OldShield)
}

void UNRAS_Character::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Character, MaxShield, OldMaxShield)
}
