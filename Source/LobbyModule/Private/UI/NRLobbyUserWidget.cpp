// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NRLobbyUserWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Subsystem/OnlineSession/OnlineSessionSubsystem.h"
#include "UI/NRLobbyServerListItem.h"
#include "OnlineSessionSettings.h"
#include "Components/TextBlock.h"

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
			OnlineSessionSubsystem->OnCreateAndStartSessionCompleteEvent.AddUObject(this, &ThisClass::OnCreateAndStartSessionComplete);
			OnlineSessionSubsystem->OnFindSessionsCompleteEvent.AddUObject(this, &ThisClass::OnFindSessionsComplete);
			OnlineSessionSubsystem->OnJoinSessionCompleteEvent.AddUObject(this, &ThisClass::OnJoinSessionComplete);
		}
	}
}

void UNRLobbyUserWidget::UnBindExternalEvent()
{
	if (OnlineSessionSubsystem)
	{
		OnlineSessionSubsystem->OnCreateAndStartSessionCompleteEvent.RemoveAll(this);
		OnlineSessionSubsystem->OnFindSessionsCompleteEvent.RemoveAll(this);
	}
	
	Super::UnBindExternalEvent();
}

void UNRLobbyUserWidget::OnButton_NewGameClicked()
{
	if (OnlineSessionSubsystem)
	{
		OnlineSessionSubsystem->CreateSession();
	}
}

void UNRLobbyUserWidget::OnButton_SearchSessionClicked()
{
	Border_ServerList->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	ScrollBox_ServerList->ClearChildren();
	Text_FindSession->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	Text_FindSession->SetText(FText::FromString(TEXT("搜寻中...")));
	
	if (OnlineSessionSubsystem)
	{
		OnlineSessionSubsystem->FindSessions(10);
	}
}

void UNRLobbyUserWidget::OnButton_QuitGameClicked()
{
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
}

void UNRLobbyUserWidget::OnCreateAndStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Create And Start Session:: bWasSuccessful:%d SessionName:%s"), bWasSuccessful, *SessionName.ToString()));

	if (UWorld* World = GetWorld())
	{
		World->ServerTravel("/Game/Lobby/Lobby?listen");
	}
}

void UNRLobbyUserWidget::OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful)
{
	if (SearchResults.Num() > 0)
	{
		Text_FindSession->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Text_FindSession->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		Text_FindSession->SetText(FText::FromString(TEXT("无")));
	}
	
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Find Session:: bWasSuccessful:%d Num:%d"), bWasSuccessful, SearchResults.Num()));
	for (const FOnlineSessionSearchResult& it: SearchResults)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan,
			FString::Printf(TEXT("Session:: ServerName:%s, Player:%d/%d, Ping:%dms"),
				*it.Session.OwningUserName,
				it.Session.SessionSettings.NumPublicConnections - it.Session.NumOpenPublicConnections,
				it.Session.SessionSettings.NumPublicConnections,
				it.PingInMs
				)
			);
		
		if (NRLobbyServerListItemClass)
		{
			if (UNRLobbyServerListItem* tItem = CreateWidget<UNRLobbyServerListItem>(GetOwningPlayer(), NRLobbyServerListItemClass))
			{
				tItem->Update(it, FOnServerListItemClickedCallBack::CreateLambda([this, it]()->void
				{
					OnlineSessionSubsystem->JoinSession(it);
				}));
				ScrollBox_ServerList->AddChild(tItem);
			}
		}
	}
}

void UNRLobbyUserWidget::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type bWasSuccessful, const FString& Address)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::Printf(TEXT("Join Session:: bWasSuccessful:%d SessionName:%s Address:%s"), bWasSuccessful, *SessionName.ToString(), *Address));

	if (bWasSuccessful == EOnJoinSessionCompleteResult::Type::Success)
	{
		if (APlayerController* PlayerController = GetOwningPlayer())
		{
			PlayerController->ClientTravel(Address, TRAVEL_Absolute);
		}
	}
}
