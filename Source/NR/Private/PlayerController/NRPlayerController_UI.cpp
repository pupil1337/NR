// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/NRPlayerController.h"

#include "NRGameSingleton.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "UI/NRUIUserWidget.h"

void ANRPlayerController::CreateUIUserWidget()
{
	if (!UIUserWidget)
	{
		if (const UNRGameSingleton* NRGameSingleton = UNRGameSingleton::Get())
		{
			if (NRGameSingleton->UIWidgetClass)
			{
				UIUserWidget = CreateWidget<UNRUIUserWidget>(this, NRGameSingleton->UIWidgetClass);
				UIUserWidget->AddToViewport();
			}
		}
	}
}

void ANRPlayerController::OnLoseInteraction(const FGameplayAbilityTargetDataHandle& Data)
{
	if (UIUserWidget)
	{
		UIUserWidget->OnLoseInteraction(Data.Get(0)->GetHitResult()->GetActor());
	}
}

void ANRPlayerController::OnFindInteraction(const FGameplayAbilityTargetDataHandle& Data)
{
	if (UIUserWidget)
	{
		UIUserWidget->OnFindInteraction(Data.Get(0)->GetHitResult()->GetActor());
	}
}

