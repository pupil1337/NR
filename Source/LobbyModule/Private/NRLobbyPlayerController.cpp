﻿// Fill out your copyright notice in the Description page of Project Settings.


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
			}
			NRGameInstance->LobbyWidget->AddToViewport();
		}

		const FInputModeUIOnly InputModeUIOnly;
		SetInputMode(InputModeUIOnly);
		SetShowMouseCursor(true);
	}

	// TODO: PlayerState中的HandleWelcomeMessage()函数
}

void ANRLobbyPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
}

void ANRLobbyPlayerController::Destroyed()
{
	Super::Destroyed();
}

void ANRLobbyPlayerController::Client_OnPlayerLogin_Implementation()
{
	int a = 1;
}
