// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/NRLifeUserWidget.h"

#include "Character/NRCharacter.h"
#include "Components/ProgressBar.h"

void UNRLifeUserWidget::BindWidgetEvent()
{
	Super::BindWidgetEvent();

	if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(GetOwningPlayerPawn()))
	{
		if (UAbilitySystemComponent* ASC = NRCharacter->GetAbilitySystemComponent())
		{
			if (const UNRAS_Character* AS_Character = Cast<UNRAS_Character>(ASC->GetAttributeSet(UNRAS_Character::StaticClass())))
			{
				// 1. Update UI
				UpdateHealth(0.0f, AS_Character->GetHealth(), AS_Character->GetMaxHealth());
				UpdateShield(0.0f, AS_Character->GetShield(), AS_Character->GetMaxShield());

				// 2. Bind Attribute Changed
				// Health
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Character->GetHealthAttribute()).AddLambda([AS_Character, this](const FOnAttributeChangeData& Data)
				{
					UpdateHealth(Data.OldValue, Data.NewValue, AS_Character->GetMaxHealth());
				});

				// MaxHealth
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Character->GetMaxHealthAttribute()).AddLambda([AS_Character, this](const FOnAttributeChangeData& Data)
				{
					UpdateMaxHealth(Data.OldValue, Data.NewValue, AS_Character->GetHealth());
				});

				// Shield
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Character->GetShieldAttribute()).AddLambda([AS_Character, this](const FOnAttributeChangeData& Data)
				{
					UpdateShield(Data.OldValue, Data.NewValue, AS_Character->GetMaxShield());
				});

				// MaxShield
				ASC->GetGameplayAttributeValueChangeDelegate(AS_Character->GetMaxShieldAttribute()).AddLambda([AS_Character, this](const FOnAttributeChangeData& Data)
				{
					UpdateMaxShield(Data.OldValue, Data.NewValue, AS_Character->GetShield());
				});
			}
		}
	}
}

void UNRLifeUserWidget::UpdateHealth(float OldHealth, float NewHealth, float MaxHealth) const
{
	if (MaxHealth <= 0.0f)
	{
		check(false)
		ProgressBar_Health->SetPercent(0.0f);
		return;
	}

	const float Health = FMath::Clamp<float>(NewHealth, 0.0f, MaxHealth);
	ProgressBar_Health->SetPercent(Health / MaxHealth);
}

void UNRLifeUserWidget::UpdateMaxHealth(float OldMaxHealth, float NewMaxHealth, float Health) const
{
	if (NewMaxHealth <= 0.0f)
	{
		check(false)
		ProgressBar_Health->SetPercent(0.0f);
		return;
	}

	Health = FMath::Clamp<float>(Health, 0.0f, NewMaxHealth);
	ProgressBar_Health->SetPercent(Health / NewMaxHealth);
}

void UNRLifeUserWidget::UpdateShield(float OldShield, float NewShield, float MaxShield) const
{
	if (MaxShield <= 0.0f)
	{
		check(false)
		ProgressBar_Shield->SetPercent(0.0f);
		return;
	}

	const float Shield = FMath::Clamp<float>(NewShield, 0.0f, MaxShield);
	ProgressBar_Shield->SetPercent(Shield / MaxShield);
}

void UNRLifeUserWidget::UpdateMaxShield(float OldMaxShield, float NewMaxShield, float Shield) const
{
	if (NewMaxShield <= 0.0f)
	{
		check(false)
		ProgressBar_Shield->SetPercent(0.0f);
		return;
	}

	Shield = FMath::Clamp<float>(Shield, 0.0f, NewMaxShield);
	ProgressBar_Shield->SetPercent(Shield / NewMaxShield);
}
