// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/OnlineSession/OnlineSessionSubsystem.h"

#include "OnlineSubsystem.h"

void UOnlineSessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (const IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get())
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("OnlineSubsystem is: %s"), *OnlineSubsystem->GetSubsystemName().ToString()));
	}

	OnCreateSessionCompleteDelegate.BindUObject(this, &ThisClass::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate.BindUObject(this, &ThisClass::OnStartSessionComplete);
	OnFindSessionsCompleteDelegate.BindUObject(this, &ThisClass::OnFindSessionsComplete);
	OnJoinSessionCompleteDelegate.BindUObject(this, &ThisClass::OnJoinSessionComplete);
}

void UOnlineSessionSubsystem::Deinitialize()
{
	if (OnlineSessionInterface)
	{
		if (OnlineSessionInterface->GetNamedSession(NAME_GameSession))
		{
			OnlineSessionInterface->DestroySession(NAME_GameSession);
		}
	}
	
	Super::Deinitialize();
}

void UOnlineSessionSubsystem::CreateSession()
{
	if (OnlineSessionInterface)
	{
		OnCreateSessionCompleteDelegateHandle = OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

		OnlineSessionSettingsPtr = MakeShareable(new FOnlineSessionSettings);
		OnlineSessionSettingsPtr->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
		OnlineSessionSettingsPtr->NumPublicConnections = 4;
		OnlineSessionSettingsPtr->bShouldAdvertise = true;
		OnlineSessionSettingsPtr->bAllowJoinInProgress = true;
		OnlineSessionSettingsPtr->bUsesPresence = true;
		OnlineSessionSettingsPtr->bAllowJoinViaPresence = true;
		OnlineSessionSettingsPtr->Set(FName("GameName"), FString("NR"), EOnlineDataAdvertisementType::Type::ViaOnlineServiceAndPing);
		OnlineSessionInterface->CreateSession(*GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId(), NAME_GameSession, *OnlineSessionSettingsPtr);
		return;
	}
	OnCreateAndStartSessionCompleteEvent.Broadcast(NAME_GameSession, false);
}

void UOnlineSessionSubsystem::FindSessions(int32 MaxSearchResults)
{
	if (OnlineSessionInterface)
	{
		OnFindSessionsCompleteDelegateHandle = OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);

		OnlineSessionSearchPtr = MakeShareable(new FOnlineSessionSearch);
		OnlineSessionSearchPtr->MaxSearchResults = MaxSearchResults;
		OnlineSessionSearchPtr->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
		OnlineSessionSearchPtr->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Type::Equals);
		OnlineSessionInterface->FindSessions(*GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId(), OnlineSessionSearchPtr.ToSharedRef());
		return;
	}
	
	OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
}

void UOnlineSessionSubsystem::JoinSession(const FOnlineSessionSearchResult& Result)
{
	if (OnlineSessionInterface)
	{
		OnJoinSessionCompleteDelegateHandle = OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

		OnlineSessionInterface->JoinSession(*GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId(), NAME_GameSession, Result);
		return;
	}

	OnJoinSessionCompleteEvent.Broadcast(NAME_GameSession, EOnJoinSessionCompleteResult::Type::UnknownError, FString());
}

void UOnlineSessionSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSessionInterface)
	{
		OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

		if (bWasSuccessful)
		{
			OnStartSessionCompleteDelegateHandle = OnlineSessionInterface->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);

			OnlineSessionInterface->StartSession(NAME_GameSession);
			return;
		}
	}

	if (!bWasSuccessful)
	{
		OnCreateAndStartSessionCompleteEvent.Broadcast(SessionName, false);	
	}
}

void UOnlineSessionSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSessionInterface)
	{
		OnlineSessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
	}

	OnCreateAndStartSessionCompleteEvent.Broadcast(SessionName, bWasSuccessful);
}

void UOnlineSessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (OnlineSessionInterface)
	{
		OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}
	
	OnFindSessionsCompleteEvent.Broadcast(OnlineSessionSearchPtr->SearchResults, bWasSuccessful);
}

void UOnlineSessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type bWasSuccessful)
{
	FString Address;
	
	if (OnlineSessionInterface)
	{
		OnlineSessionInterface->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);
		
		OnlineSessionInterface->GetResolvedConnectString(SessionName, Address);
	}

	OnJoinSessionCompleteEvent.Broadcast(SessionName, bWasSuccessful, Address);
}
