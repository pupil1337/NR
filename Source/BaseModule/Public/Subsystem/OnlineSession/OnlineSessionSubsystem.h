// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSessionSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCreateSessionCompleteEvent, FName, bool)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnDestroySessionCompleteEvent, FName, bool)
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFindSessionsCompleteEvent, const TArray<FOnlineSessionSearchResult>&, bool);

/**
 * 
 */
UCLASS()
class BASEMODULE_API UOnlineSessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
//~Begin This Class
	void CreateSession();
	void DestroySession();
	void FindSessions(int32 MaxSearchResults);

	FORCEINLINE const TArray<FOnlineSessionSearchResult>& GetSearchResults() const { return OnlineSessionSearchPtr->SearchResults; }

	FOnCreateSessionCompleteEvent OnCreateSessionCompleteEvent;
	FOnDestroySessionCompleteEvent OnDestroySessionCompleteEvent;
	FOnFindSessionsCompleteEvent OnFindSessionsCompleteEvent;
	
private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	
	IOnlineSessionPtr OnlineSessionInterface;
	TSharedPtr<FOnlineSessionSettings> OnlineSessionSettingsPtr;
	TSharedPtr<FOnlineSessionSearch> OnlineSessionSearchPtr;

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	// Temp
	bool bCreateSessionAfterDestroyed = false;
};
