// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NRLobbyUserWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

void UNRLobbyUserWidget::BindWidgetEvent()
{
	Super::BindWidgetEvent();
	
	Button_NewGame->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButton_NewGameClicked);
	Button_SearchSession->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButton_SearchSessionClicked);
	Button_QuitGame->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButton_QuitGameClicked);
}

void UNRLobbyUserWidget::OnButton_NewGameClicked()
{
	GetWorld()->ServerTravel("/Game/Maps/TestMain?listen");
}

void UNRLobbyUserWidget::OnButton_SearchSessionClicked()
{
	Border_ServerList->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Border_ServerList->ClearChildren();
}

void UNRLobbyUserWidget::OnButton_QuitGameClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}
