// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GAS/Attribute/NRAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "Types/NRGASTypes.h"
#include "Net/UnrealNetwork.h"

UNRAttributeSet::UNRAttributeSet()
{
}

void UNRAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UNRAttributeSet, Health, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAttributeSet, Shield, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UNRAttributeSet, MaxShield, COND_None, REPNOTIFY_Always)
}

void UNRAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	// TODO
}

void UNRAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
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

void UNRAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAttributeSet, Health, OldHealth)
}

void UNRAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAttributeSet, MaxHealth, OldMaxHealth)
}

void UNRAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAttributeSet, Shield, OldShield)
}

void UNRAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UNRAttributeSet, MaxShield, OldMaxShield)
}
