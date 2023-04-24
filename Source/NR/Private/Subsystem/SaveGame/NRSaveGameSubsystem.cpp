// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/SaveGame/NRSaveGameSubsystem.h"

#include "Subsystem/SaveGame/NRSaveGame.h"

void UNRSaveGameSubsystem::SaveGame(const FAsyncSaveGameToSlotDelegate& SavedDelegate) const
{
	if (UNRSaveGame* tNRSaveGame = Cast<UNRSaveGame>(UGameplayStatics::CreateSaveGameObject(UNRSaveGame::StaticClass())))
	{
		if (const APlayerController* PlayerController = GetLocallyPlayerController())
		{
			tNRSaveGame->PlayerName = TEXT("Pupil");
			tNRSaveGame->Location = PlayerController->GetFocalLocation();

			UGameplayStatics::AsyncSaveGameToSlot(tNRSaveGame, TEXT("SavedGame"), 0, SavedDelegate);	
		}
	}
}

void UNRSaveGameSubsystem::LoadGame(const FAsyncLoadGameFromSlotDelegate& LoadedDelegate)
{
	UGameplayStatics::AsyncLoadGameFromSlot(TEXT("SavedGame"), 0, LoadedDelegate);
}

APlayerController* UNRSaveGameSubsystem::GetLocallyPlayerController() const
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		for (const ULocalPlayer* it: GameInstance->GetLocalPlayers())
		{
			if (it && it->PlayerController && it->PlayerController->IsLocalPlayerController())
			{
				return it->PlayerController;
			}
		}
	}
	return nullptr;
}
