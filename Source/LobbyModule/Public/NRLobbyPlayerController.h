// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NRLobbyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LOBBYMODULE_API ANRLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

//~Begin This Class
public:
	UFUNCTION(Client, Reliable)
	void Client_OnPlayerLogin();
};
