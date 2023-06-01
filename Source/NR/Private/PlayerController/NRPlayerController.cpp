// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/NRPlayerController.h"

#include "NRGameSingleton.h"
#include "NRUserWidgetBase.h"
#include "UI/NRUIUserWidget.h"


void ANRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		const FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);
		SetShowMouseCursor(false);

		CreateUIUserWidget();
	}
}

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

