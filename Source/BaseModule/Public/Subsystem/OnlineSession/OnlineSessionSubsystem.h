// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSessionSubsystem.generated.h"

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
	void CreateSession(const FOnCreateSessionCompleteDelegate& OnCreateSessionCompleteDelegate);
	void DestroySession(const FOnDestroySessionCompleteDelegate& OnDestroySessionCompleteDelegate);
	void FindSessions(int32 MaxSearchResults, const FOnFindSessionsCompleteDelegate& OnFindSessionsCompleteDelegate);

	FORCEINLINE const TArray<FOnlineSessionSearchResult>& GetSearchResults() const { return OnlineSessionSearchPtr->SearchResults; }
private:
	IOnlineSessionPtr OnlineSessionInterface;
	TSharedPtr<FOnlineSessionSettings> OnlineSessionSettingsPtr;
	TSharedPtr<FOnlineSessionSearch> OnlineSessionSearchPtr;
	
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;

	// Temp
	bool bCreateSessionAfterDestroyed = false;
};
