// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NRLobbyServerListItem.h"

#include "OnlineSessionSettings.h"
#include "Components/TextBlock.h"

void UNRLobbyServerListItem::Update(const FOnlineSessionSearchResult& SearchResult) const
{
	const int32 MaxPlayerNum = SearchResult.Session.SessionSettings.NumPublicConnections;
	const int32 CurrPlayerNum = SearchResult.Session.SessionSettings.NumPublicConnections - SearchResult.Session.NumOpenPublicConnections;
	
	Text_ServerName->SetText(FText::FromString(SearchResult.Session.OwningUserName));
	Text_PlayerNum->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), CurrPlayerNum, MaxPlayerNum)));
	Text_Ping->SetText(FText::FromString(FString::Printf(TEXT("%d ms"), SearchResult.PingInMs)));
}
