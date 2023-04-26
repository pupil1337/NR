// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/NRPlayerController.h"


void ANRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		const FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);
		
		SetShowMouseCursor(false);
	}
}

