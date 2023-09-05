// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Interact/NRInteractMonsterLifeUserWidget.h"

#include "AbilitySystemComponent.h"
#include "AI/NRMonster.h"
#include "AI/GAS/NRAS_Monster.h"
#include "Components/ProgressBar.h"

void UNRInteractMonsterLifeUserWidget::OnLoseMonster()
{
	if (Monster.IsValid())
	{
		if (UAbilitySystemComponent* ASC = Monster.Get()->GetAbilitySystemComponent())
		{
			if (const UNRAS_Monster* AS_Monster = Cast<UNRAS_Monster>(ASC->GetAttributeSet(UNRAS_Monster::StaticClass())))
			{
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Monster->GetArmorAttribute()).Remove(ArmorHandle);
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Monster->GetMaxArmorAttribute()).Remove(MaxArmorHandle);
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Monster->GetShieldAttribute()).Remove(ShieldHandle);
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Monster->GetMaxShieldAttribute()).Remove(MaxShieldHandle);
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Monster->GetHealthAttribute()).Remove(HealthHandle);
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Monster->GetMaxHealthAttribute()).Remove(MaxHealthHandle);
			}
		}
		Monster.Reset();
	}

	SetVisibility(ESlateVisibility::Collapsed);
}

void UNRInteractMonsterLifeUserWidget::OnFindMonster(ANRMonster* InMonster)
{
	if (InMonster && InMonster != Monster.Get())
	{
		OnLoseMonster();

		Monster = InMonster;
		if (UAbilitySystemComponent* ASC = Monster.Get()->GetAbilitySystemComponent())
		{
			if (const UNRAS_Monster* AS_Monster = Cast<UNRAS_Monster>(ASC->GetAttributeSet(UNRAS_Monster::StaticClass())))
			{
				// 1. Update UI
				UpdateArmor(0.0f, AS_Monster->GetArmor(), AS_Monster->GetMaxArmor());
				UpdateShield(0.0f, AS_Monster->GetShield(), AS_Monster->GetMaxShield());
				UpdateHealth(0.0f, AS_Monster->GetHealth(), AS_Monster->GetMaxHealth());
		
				// 2. Bind Attribute Changed
				// Armor
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Monster->GetArmorAttribute()).AddLambda([this, AS_Monster](const FOnAttributeChangeData& Data)
				{
					UpdateArmor(Data.OldValue, Data.NewValue, AS_Monster->GetMaxArmor());
				});
		
				// MaxArmor
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Monster->GetMaxArmorAttribute()).AddLambda([this, AS_Monster](const FOnAttributeChangeData& Data)
				{
					UpdateMaxArmor(Data.OldValue, Data.NewValue, AS_Monster->GetArmor());
				});
		
				// Shield
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Monster->GetShieldAttribute()).AddLambda([this, AS_Monster](const FOnAttributeChangeData& Data)
				{
					UpdateShield(Data.OldValue, Data.NewValue, AS_Monster->GetMaxShield());
				});
		
				// MaxShield
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Monster->GetMaxShieldAttribute()).AddLambda([this, AS_Monster](const FOnAttributeChangeData& Data)
				{
					UpdateMaxShield(Data.OldValue, Data.NewValue, AS_Monster->GetShield());
				});
		
				// Health
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Monster->GetHealthAttribute()).AddLambda([this, AS_Monster](const FOnAttributeChangeData& Data)
				{
					UpdateHealth(Data.OldValue, Data.NewValue, AS_Monster->GetMaxHealth());
				});
		
				// MaxHealth
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Monster->GetMaxHealthAttribute()).AddLambda([this, AS_Monster](const FOnAttributeChangeData& Data)
				{
					UpdateMaxHealth(Data.OldValue, Data.NewValue, AS_Monster->GetHealth());
				});
			}
		}
		
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void UNRInteractMonsterLifeUserWidget::UpdateArmor(float OldArmor, float NewArmor, float MaxArmor)
{
	if (MaxArmor <= 0.0f)
	{
		ProgressBar_Armor->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const float Armor = FMath::Clamp<float>(NewArmor, 0.0f, MaxArmor);
	ProgressBar_Armor->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ProgressBar_Armor->SetPercent(Armor / MaxArmor);
}

void UNRInteractMonsterLifeUserWidget::UpdateMaxArmor(float OldMaxArmor, float NewMaxArmor, float Armor)
{
	if (NewMaxArmor <= 0.0f)
	{
		ProgressBar_Armor->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	Armor = FMath::Clamp<float>(Armor, 0.0f, NewMaxArmor);
	ProgressBar_Armor->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ProgressBar_Armor->SetPercent(Armor / NewMaxArmor);
}

void UNRInteractMonsterLifeUserWidget::UpdateShield(float OldShield, float NewShield, float MaxShield)
{
	if (MaxShield <= 0.0f)
	{
		ProgressBar_Shield->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const float Shield = FMath::Clamp<float>(NewShield, 0.0f, MaxShield);
	ProgressBar_Shield->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ProgressBar_Shield->SetPercent(Shield / MaxShield);
}

void UNRInteractMonsterLifeUserWidget::UpdateMaxShield(float OldMaxShield, float NewMaxShield, float Shield)
{
	if (NewMaxShield <= 0.0f)
	{
		ProgressBar_Shield->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	Shield = FMath::Clamp<float>(Shield, 0.0f, NewMaxShield);
	ProgressBar_Shield->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ProgressBar_Shield->SetPercent(Shield / NewMaxShield);
}

void UNRInteractMonsterLifeUserWidget::UpdateHealth(float OldHealth, float NewHealth, float MaxHealth)
{
	if (MaxHealth <= 0.0f)
	{
		ProgressBar_Health->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	const float Health = FMath::Clamp<float>(NewHealth, 0.0f, MaxHealth);
	ProgressBar_Health->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ProgressBar_Health->SetPercent(Health / MaxHealth);
}

void UNRInteractMonsterLifeUserWidget::UpdateMaxHealth(float OldMaxHealth, float NewMaxHealth, float Health)
{
	if (NewMaxHealth <= 0.0f)
	{
		ProgressBar_Health->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	Health = FMath::Clamp<float>(Health, 0.0f, NewMaxHealth);
	ProgressBar_Health->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ProgressBar_Health->SetPercent(Health / NewMaxHealth);
}
