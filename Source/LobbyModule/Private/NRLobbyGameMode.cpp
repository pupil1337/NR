// Fill out your copyright notice in the Description page of Project Settings.


#include "NRLobbyGameMode.h"

#include "NRLobbyPlayerController.h"

void ANRLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (ANRLobbyPlayerController* NRLobbyPlayerController = Cast<ANRLobbyPlayerController>(NewPlayer))
	{
		NRLobbyPlayerController->Client_OnPlayerLogin();
		
		PlayerControllers.Push(NRLobbyPlayerController);
	}
}

void ANRLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
}
