// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NRLobbyUserWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystem/OnlineSession/OnlineSessionSubsystem.h"

void UNRLobbyUserWidget::BindWidgetEvent()
{
	Super::BindWidgetEvent();
	
	Button_NewGame->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButton_NewGameClicked);
	Button_SearchSession->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButton_SearchSessionClicked);
	Button_QuitGame->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButton_QuitGameClicked);


	OnCreateSessionCompleteDelegate.BindUObject(this, &ThisClass::OnCreateSessionComplete);
	OnFindSessionsCompleteDelegate.BindUObject(this, &ThisClass::OnFindSessionsComplete);
}

void UNRLobbyUserWidget::OnButton_NewGameClicked()
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UOnlineSessionSubsystem* OnlineSessionSubsystem = GameInstance->GetSubsystem<UOnlineSessionSubsystem>())
		{
			OnlineSessionSubsystem->CreateSession(OnCreateSessionCompleteDelegate);
		}
	}
	//GetWorld()->ServerTravel("/Game/Maps/TestMain?listen");
}

void UNRLobbyUserWidget::OnButton_SearchSessionClicked()
{
	Border_ServerList->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Border_ServerList->ClearChildren();

	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (UOnlineSessionSubsystem* OnlineSessionSubsystem = GameInstance->GetSubsystem<UOnlineSessionSubsystem>())
		{
			OnlineSessionSubsystem->FindSessions(100, OnFindSessionsCompleteDelegate);
		}
	}
}

void UNRLobbyUserWidget::OnButton_QuitGameClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

void UNRLobbyUserWidget::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Create Session:: bWasSuccessful:%d SessionName:%s"), bWasSuccessful, *SessionName.ToString()));
}

void UNRLobbyUserWidget::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (const UOnlineSessionSubsystem* OnlineSessionSubsystem = GameInstance->GetSubsystem<UOnlineSessionSubsystem>())
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Find Session:: bWasSuccessful:%d Num:%d"), bWasSuccessful, OnlineSessionSubsystem->GetSearchResults().Num()));
		}
	}
}
