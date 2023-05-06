// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSessionSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCreateAndStartSessionCompleteEvent, FName, bool)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFindSessionsCompleteEvent, const TArray<FOnlineSessionSearchResult>&, bool)
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnJoinSessionCompleteEvent, FName, EOnJoinSessionCompleteResult::Type, const FString&)

/**
 * 
 */
UCLASS()
class BASEMODULE_API UOnlineSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
//~Begin This Class
	FOnCreateAndStartSessionCompleteEvent OnCreateAndStartSessionCompleteEvent;
	FOnFindSessionsCompleteEvent OnFindSessionsCompleteEvent;
	FOnJoinSessionCompleteEvent OnJoinSessionCompleteEvent;
	
	void CreateSession();
	void FindSessions(int32 MaxSearchResults);
	void JoinSession(const FOnlineSessionSearchResult& Result);
	
private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type bWasSuccessful);
	
	IOnlineSessionPtr OnlineSessionInterface;
	TSharedPtr<FOnlineSessionSettings> OnlineSessionSettingsPtr;
	TSharedPtr<FOnlineSessionSearch> OnlineSessionSearchPtr;

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;
	
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;
};
