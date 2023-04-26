// Fill out your copyright notice in the Description page of Project Settings.


#include "NRLobbyUserWidget.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UNRLobbyUserWidget::BindWidgetEvent()
{
	Super::BindWidgetEvent();

	if (Button_NewGame)
	{
		Button_NewGame->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButton_NewGameClicked);
	}
	if (Button_QuitGame)
	{
		Button_QuitGame->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButton_QuitGameClicked);
	}
}

void UNRLobbyUserWidget::OnButton_NewGameClicked()
{
	GetWorld()->ServerTravel("/Game/Maps/TestMain?listen");
}

void UNRLobbyUserWidget::OnButton_QuitGameClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}
