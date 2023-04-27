// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NRLobbyUserWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystem/OnlineSession/OnlineSessionSubsystem.h"
#include "UI/NRLobbyServerListItem.h"

void UNRLobbyUserWidget::BindWidgetEvent()
{
	Super::BindWidgetEvent();
	
	Button_NewGame->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButton_NewGameClicked);
	Button_SearchSession->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButton_SearchSessionClicked);
	Button_QuitGame->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButton_QuitGameClicked);

	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		OnlineSessionSubsystem = GameInstance->GetSubsystem<UOnlineSessionSubsystem>();

		if (OnlineSessionSubsystem)
		{
			OnlineSessionSubsystem->OnCreateSessionCompleteEvent.AddUObject(this, &ThisClass::OnCreateSessionComplete);
			OnlineSessionSubsystem->OnFindSessionsCompleteEvent.AddUObject(this, &ThisClass::OnFindSessionsComplete);
		}
	}
	
}

void UNRLobbyUserWidget::OnButton_NewGameClicked()
{
	if (OnlineSessionSubsystem)
	{
		OnlineSessionSubsystem->CreateSession();
	}
	//GetWorld()->ServerTravel("/Game/Maps/TestMain?listen");
}

void UNRLobbyUserWidget::OnButton_SearchSessionClicked()
{
	Border_ServerList->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Border_ServerList->ClearChildren();

	if (OnlineSessionSubsystem)
	{
		OnlineSessionSubsystem->FindSessions(100);
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

void UNRLobbyUserWidget::OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Find Session:: bWasSuccessful:%d Num:%d"), bWasSuccessful, SearchResults.Num()));
	for (const FOnlineSessionSearchResult& it: SearchResults)
	{
		if (UNRLobbyServerListItem* tItem = CreateWidget<UNRLobbyServerListItem>(GetOwningPlayer(), UNRLobbyServerListItem::StaticClass()))
		{
			tItem->Update(it);
			ScrollBox_ServerList->AddChild(tItem);
		}
	}
}
