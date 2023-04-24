// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/SaveGame/NRSaveGameSubsystem.h"

#include "Subsystem/SaveGame/NRSaveGame.h"

void UNRSaveGameSubsystem::SaveGame(const FAsyncSaveGameToSlotDelegate& SavedDelegate)
{
	if (UNRSaveGame* NRSaveGame = Cast<UNRSaveGame>(UGameplayStatics::CreateSaveGameObject(UNRSaveGame::StaticClass())))
	{
		if (APlayerController* PlayerController = GetLocallyPlayerController())
		{
			NRSaveGame->PlayerName = TEXT("Pupil");
			NRSaveGame->Location = PlayerController->GetFocalLocation();

			UGameplayStatics::AsyncSaveGameToSlot(NRSaveGame, TEXT("SaveGameTest"), 0, SavedDelegate);	
		}
	}
}

void UNRSaveGameSubsystem::LoadGame(const FAsyncLoadGameFromSlotDelegate& LoadedDelegate)
{
	UGameplayStatics::AsyncLoadGameFromSlot(TEXT("SaveGameTest"), 0, LoadedDelegate);
}

APlayerController* UNRSaveGameSubsystem::GetLocallyPlayerController() const
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		for (ULocalPlayer* it: GameInstance->GetLocalPlayers())
		{
			if (it && it->PlayerController && it->PlayerController->IsLocalPlayerController())
			{
				return it->PlayerController;
			}
		}
	}
	return nullptr;
}
