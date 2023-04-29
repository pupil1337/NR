// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "OnlineSessionSubsystem.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCreateAndStartSessionCompleteEvent, FName, bool)
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
	virtual void Deinitialize() override;
	
//~Begin This Class
	FOnCreateAndStartSessionCompleteEvent OnCreateAndStartSessionCompleteEvent;
	FOnFindSessionsCompleteEvent OnFindSessionsCompleteEvent;
	
	void CreateSession();
	void FindSessions(int32 MaxSearchResults);
	
private:
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	
	IOnlineSessionPtr OnlineSessionInterface;
	TSharedPtr<FOnlineSessionSettings> OnlineSessionSettingsPtr;
	TSharedPtr<FOnlineSessionSearch> OnlineSessionSearchPtr;

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
	
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
};
