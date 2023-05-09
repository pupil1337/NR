// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NRLobbyGameMode.generated.h"

class ANRLobbyPlayerController;

/**
 * 
 */
UCLASS()
class LOBBYMODULE_API ANRLobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;

//~Begin This Class
private:
	TArray<ANRLobbyPlayerController*> PlayerControllers;
};
