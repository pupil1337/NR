// Fill out your copyright notice in the Description page of Project Settings.


#include "NRLobbyPlayerController.h"

#include "NRGameInstance.h"
#include "UI/NRLobbyUserWidget.h"

void ANRLobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		if (UNRGameInstance* NRGameInstance = GetGameInstance<UNRGameInstance>())
		{
			if (!NRGameInstance->LobbyWidget)
			{
				NRGameInstance->LobbyWidget = CreateWidget<UNRLobbyUserWidget>(NRGameInstance, NRLobbyUserWidgetClass);
				NRGameInstance->LobbyWidget->AddToViewport();
			}
		}

		const FInputModeUIOnly InputModeUIOnly;
		SetInputMode(InputModeUIOnly);
		SetShowMouseCursor(true);
	}
}

void ANRLobbyPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
}

void ANRLobbyPlayerController::Destroyed()
{
	if (UNRGameInstance* NRGameInstance = GetGameInstance<UNRGameInstance>())
	{
		if (UNRLobbyUserWidget* NRLobbyUserWidget = Cast<UNRLobbyUserWidget>(NRGameInstance->LobbyWidget))
		{
			NRLobbyUserWidget->RemoveFromParentImpl();
			NRGameInstance->LobbyWidget = nullptr;
		}
	}
	Super::Destroyed();
}

void ANRLobbyPlayerController::Client_OnPlayerLogin_Implementation()
{
}
