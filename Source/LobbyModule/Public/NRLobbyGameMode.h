// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NRLobbyGameMode.generated.h"

UCLASS()
class LOBBYMODULE_API ANRLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANRLobbyGameMode();

protected:
	virtual void BeginPlay() override;
};
