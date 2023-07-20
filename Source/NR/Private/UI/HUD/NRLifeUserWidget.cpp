// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/NRLifeUserWidget.h"

#include "Character/NRCharacter.h"
#include "Components/ProgressBar.h"

void UNRLifeUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (APawn* Pawn = GetOwningPlayerPawn())
	{
		if (const ANRCharacter* NRCharacter = Cast<ANRCharacter>(Pawn))
		{
			// Health
			const float Health = NRCharacter->GetHealth();
			const float MaxHealth = NRCharacter->GetMaxHealth();
			if (MaxHealth > 0.0f && Health > 0.0f)
			{
				ProgressBar_Health->SetPercent(Health / MaxHealth);
			}
			else
			{
				ProgressBar_Health->SetPercent(0.0f);
			}

			// Shield
			const float Shield = NRCharacter->GetShield();
			const float MaxShield = NRCharacter->GetMaxShield();
			if (MaxShield > 0.0f && Shield > 0.0f)
			{
				ProgressBar_Shield->SetPercent(Shield / MaxShield);
			}
			else
			{
				ProgressBar_Shield->SetPercent(0.f);
			}
		}
	}
}
