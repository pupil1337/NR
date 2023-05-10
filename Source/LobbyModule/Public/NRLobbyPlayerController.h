// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NRLobbyPlayerController.generated.h"

class UNRLobbyUserWidget;
class ANRLobbyCharacter;

/**
 * 
 */
UCLASS()
class LOBBYMODULE_API ANRLobbyPlayerController : public APlayerController
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNRLobbyUserWidget> NRLobbyUserWidgetClass;
	
public:
	virtual void BeginPlay() override;
	virtual void AcknowledgePossession(APawn* P) override;

	virtual void Destroyed() override;
	
//~Begin This Class
	UFUNCTION(Client, Reliable)
	void Client_OnPlayerLogin();

private:
	UPROPERTY(Transient)
	ANRLobbyCharacter* NRLobbyCharacter;
};
