// Fill out your copyright notice in the Description page of Project Settings.


#include "NRLobbyGameMode.h"

#include "NRLobbyPlayerController.h"

void ANRLobbyGameMode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	if (ANRLobbyPlayerController* NRLobbyPlayerController = Cast<ANRLobbyPlayerController>(NewPlayer))
	{
		NRLobbyPlayerController->Client_OnPlayerLogin();
		
		PlayerControllers.Push(NRLobbyPlayerController);
	}
}

void ANRLobbyGameMode::Logout(AController* Exiting)
{
	if (ANRLobbyPlayerController* NRLobbyPlayerController = Cast<ANRLobbyPlayerController>(Exiting))
	{
		PlayerControllers.Remove(NRLobbyPlayerController);
	}
	
	Super::Logout(Exiting);
}

AActor* ANRLobbyGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
{
	FString ComingName = IncomingName;
	if (IncomingName.IsEmpty())
	{
		for (int i = 0; i < PlayerControllers.Num(); ++i)
		{
			if (PlayerControllers[i] == Player)
			{
				ComingName = FString::FromInt(i);
			}
		}
	}
	
	return Super::FindPlayerStart_Implementation(Player, ComingName);
}

