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
	OnDestroySessionCompleteDelegate.BindUObject(this, &ThisClass::OnDestroySessionComplete);
	OnFindSessionsCompleteDelegate.BindUObject(this, &ThisClass::OnFindSessionsComplete);
}

void UOnlineSessionSubsystem::CreateSession()
{
	if (OnlineSessionInterface)
	{
		if (OnlineSessionInterface->GetNamedSession(NAME_GameSession))
		{
			bCreateSessionAfterDestroyed = true;
			DestroySession();	
			return;
		}
		
		OnCreateSessionCompleteDelegateHandle = OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);

		OnlineSessionSettingsPtr = MakeShareable(new FOnlineSessionSettings);
		OnlineSessionSettingsPtr->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
		OnlineSessionSettingsPtr->NumPublicConnections = 4;
		OnlineSessionSettingsPtr->bAllowJoinInProgress = true;
		OnlineSessionSettingsPtr->bUsesPresence = true;
		OnlineSessionSettingsPtr->bAllowJoinViaPresence = true;
		OnlineSessionSettingsPtr->bShouldAdvertise = true;
		//OnlineSessionSettingsPtr->bUseLobbiesIfAvailable = true;
		//OnlineSessionSettingsPtr->BuildUniqueId = true;
		//OnlineSessionSettingsPtr->Set(FName("GameName"), FString("NR"), EOnlineDataAdvertisementType::Type::ViaOnlineServiceAndPing);
		if (!OnlineSessionInterface->CreateSession(*GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId(), NAME_GameSession, *OnlineSessionSettingsPtr))
		{
			OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

			OnCreateAndStartSessionCompleteEvent.Broadcast(NAME_GameSession, false);
		}
	}
}

void UOnlineSessionSubsystem::DestroySession()
{
	if (OnlineSessionInterface)
	{
		OnDestroySessionCompleteDelegateHandle = OnlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

		if (!OnlineSessionInterface->DestroySession(NAME_GameSession))
		{
			OnlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);

			OnDestroySessionCompleteEvent.Broadcast(NAME_GameSession, false);
		}
	}
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
		if (!OnlineSessionInterface->FindSessions(*GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId(), OnlineSessionSearchPtr.ToSharedRef()))
		{
			OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);

			OnFindSessionsCompleteEvent.Broadcast(TArray<FOnlineSessionSearchResult>(), false);
		}
	}
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
	OnCreateAndStartSessionCompleteEvent.Broadcast(SessionName, false);
}

void UOnlineSessionSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSessionInterface)
	{
		OnlineSessionInterface->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
	}

	OnCreateAndStartSessionCompleteEvent.Broadcast(SessionName, bWasSuccessful);
}

void UOnlineSessionSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (OnlineSessionInterface)
	{
		OnlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
	}
	OnDestroySessionCompleteEvent.Broadcast(SessionName, bWasSuccessful);

	if (bWasSuccessful && bCreateSessionAfterDestroyed)
	{
		bCreateSessionAfterDestroyed = false;
		CreateSession();
	}
}

void UOnlineSessionSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (OnlineSessionInterface)
	{
		OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
	}
	OnFindSessionsCompleteEvent.Broadcast(OnlineSessionSearchPtr->SearchResults, bWasSuccessful);
}
