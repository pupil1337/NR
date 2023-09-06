// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GAS/NRAS_Monster.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UNRAS_Monster::UNRAS_Monster()
{
}

void UNRAS_Monster::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Monster, Armor,     COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Monster, MaxArmor,  COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Monster, Shield,    COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Monster, MaxShield, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Monster, Health,    COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAS_Monster, MaxHealth, COND_None, REPNOTIFY_Always)
}

void UNRAS_Monster::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
}

void UNRAS_Monster::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		float LocalDamageCone = GetDamage();
		SetDamage(0.0f);

		// 减去护甲
		if (LocalDamageCone > 0.0f)
		{
			const float OldArmor = GetArmor();
			if (OldArmor > 0.0f)
			{
				SetArmor(FMath::Clamp<float>(OldArmor - LocalDamageCone, 0.0f, GetMaxArmor()));
				LocalDamageCone -= OldArmor;
			}
		}

		// 减去护盾
		if (LocalDamageCone > 0.0f)
		{
			const float OldShield = GetShield();
			if (OldShield > 0.0f)
			{
				SetShield(FMath::Clamp<float>(OldShield - LocalDamageCone, 0.0f, GetMaxShield()));
				LocalDamageCone -= OldShield;
			}
		}

		// 减去血量
		if (LocalDamageCone > 0.0f)
		{
			SetHealth(FMath::Clamp<float>(GetHealth() - LocalDamageCone, 0.0f, GetMaxHealth()));
		}
	}
}

void UNRAS_Monster::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Monster, Armor, OldArmor)
}

void UNRAS_Monster::OnRep_MaxArmor(const FGameplayAttributeData& OldMaxArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Monster, MaxArmor, OldMaxArmor)
}

void UNRAS_Monster::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Monster, Shield, OldShield)
}

void UNRAS_Monster::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Monster, MaxShield, OldMaxShield)
}

void UNRAS_Monster::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Monster, Health, OldHealth)
}

void UNRAS_Monster::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAS_Monster, MaxHealth, OldMaxHealth)
}
