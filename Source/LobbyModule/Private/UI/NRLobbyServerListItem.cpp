// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NRLobbyServerListItem.h"

#include "OnlineSessionSettings.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UNRLobbyServerListItem::BindWidgetEvent()
{
	Super::BindWidgetEvent();

	Button_Select->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButton_SelectClicked);
}

void UNRLobbyServerListItem::Update(const FOnlineSessionSearchResult& InSearchResult, const FOnServerListItemClickedCallBack& InOnClickedCallBack)
{
	SearchResult = InSearchResult;
	OnClickedCallBack = InOnClickedCallBack;
	
	const int32 MaxPlayerNum = SearchResult.Session.SessionSettings.NumPublicConnections;
	const int32 CurrPlayerNum = SearchResult.Session.SessionSettings.NumPublicConnections - SearchResult.Session.NumOpenPublicConnections;
	
	Text_ServerName->SetText(FText::FromString(SearchResult.Session.OwningUserName));
	Text_PlayerNum->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrPlayerNum, MaxPlayerNum)));
	Text_Ping->SetText(FText::FromString(FString::Printf(TEXT("%d ms"), SearchResult.PingInMs)));
}

void UNRLobbyServerListItem::OnButton_SelectClicked()
{
	OnClickedCallBack.Execute();
}
