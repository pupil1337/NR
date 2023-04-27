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
}

void UOnlineSessionSubsystem::CreateSession(const FOnCreateSessionCompleteDelegate& OnCreateSessionCompleteDelegate)
{
	if (OnlineSessionInterface)
	{
		if (OnlineSessionInterface->GetNamedSession(NAME_GameSession))
		{
			bCreateSessionAfterDestroyed = true;
			DestroySession(FOnDestroySessionCompleteDelegate::CreateLambda([](FName SessionName, bool bWasSuccessful)->void
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Destroy Session:: bWasSuccessful:%d SessionName:%s"), bWasSuccessful, *SessionName.ToString()));
			}));	
			return;
		}
		
		OnCreateSessionCompleteDelegateHandle = OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionCompleteDelegate::CreateLambda([this, &OnCreateSessionCompleteDelegate](FName SessionName, bool bWasSuccessful) -> void
		{
			if (OnlineSessionInterface)
			{
				OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			}
			OnCreateSessionCompleteDelegate.Execute(SessionName, bWasSuccessful);
		}));

		OnlineSessionSettingsPtr = MakeShareable(new FOnlineSessionSettings);
		OnlineSessionSettingsPtr->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
		OnlineSessionSettingsPtr->NumPublicConnections = 4;
		OnlineSessionSettingsPtr->bAllowJoinInProgress = true;
		OnlineSessionSettingsPtr->bUsesPresence = true;
		OnlineSessionSettingsPtr->bAllowJoinViaPresence = true;
		OnlineSessionSettingsPtr->bShouldAdvertise = true;
		OnlineSessionSettingsPtr->bUseLobbiesIfAvailable = true;
		OnlineSessionSettingsPtr->BuildUniqueId = true;
		OnlineSessionSettingsPtr->Set(FName("MatchType"), FString("NR"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		if (!OnlineSessionInterface->CreateSession(*GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId(), NAME_GameSession, *OnlineSessionSettingsPtr))
		{
			OnlineSessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);

			OnCreateSessionCompleteDelegate.Execute(FName("None"), false);
		}
	}
}

void UOnlineSessionSubsystem::DestroySession(const FOnDestroySessionCompleteDelegate& OnDestroySessionCompleteDelegate)
{
	if (OnlineSessionInterface)
	{
		OnDestroySessionCompleteDelegateHandle = OnlineSessionInterface->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionCompleteDelegate::CreateLambda([this, &OnDestroySessionCompleteDelegate](FName SessionName, bool bWasSuccessful)->void
		{
			if (OnlineSessionInterface)
			{
				OnlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
			}
			OnDestroySessionCompleteDelegate.Execute(SessionName, bWasSuccessful);

			if (bWasSuccessful && bCreateSessionAfterDestroyed)
			{
				bCreateSessionAfterDestroyed = false;
				CreateSession(FOnCreateSessionCompleteDelegate::CreateLambda([](FName SessionName, bool bWasSuccessful)->void
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Create Session:: bWasSuccessful:%d SessionName:%s"), bWasSuccessful, *SessionName.ToString()));
				}));
			}
		}));

		if (!OnlineSessionInterface->DestroySession(NAME_GameSession))
		{
			OnlineSessionInterface->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
			OnDestroySessionCompleteDelegate.Execute(FName("None"), false);
		}
	}
}

void UOnlineSessionSubsystem::FindSessions(int32 MaxSearchResults, const FOnFindSessionsCompleteDelegate& OnFindSessionsCompleteDelegate)
{
	if (OnlineSessionInterface)
	{
		OnFindSessionsCompleteDelegateHandle = OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FOnFindSessionsCompleteDelegate::CreateLambda([this, &OnFindSessionsCompleteDelegate](bool bWasSuccessful)->void
		{
			if (OnlineSessionInterface)
			{
				OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
			}
			OnFindSessionsCompleteDelegate.Execute(bWasSuccessful);
		}));

		OnlineSessionSearchPtr = MakeShareable(new FOnlineSessionSearch);
		OnlineSessionSearchPtr->MaxSearchResults = MaxSearchResults;
		OnlineSessionSearchPtr->bIsLanQuery = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL" ? true : false;
		OnlineSessionSearchPtr->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Type::Equals);
		if (!OnlineSessionInterface->FindSessions(*GetWorld()->GetFirstLocalPlayerFromController()->GetPreferredUniqueNetId(), OnlineSessionSearchPtr.ToSharedRef()))
		{
			OnlineSessionInterface->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
			OnFindSessionsCompleteDelegate.Execute(false);
		}
	}
}
