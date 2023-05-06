// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NRLobbyUserWidget.generated.h"

class UButton;
class UBorder;
class UScrollBox;
class UTextBlock;
class UNRLobbyServerListItem;
class UOnlineSessionSubsystem;

/**
 * 
 */
UCLASS(Blueprintable)
class LOBBYMODULE_API UNRLobbyUserWidget : public UNRUserWidgetBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_ContinueGame;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_NewGame;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_SearchSession;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Setting;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_QuitGame;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border_ServerList;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_ServerList;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_FindSession;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNRLobbyServerListItem> NRLobbyServerListItemClass;
	
protected:
	virtual void BindWidgetEvent() override;
	virtual void UnBindExternalEvent() override;

	//~Begin This Class
private:
	UFUNCTION()
	void OnButton_NewGameClicked();

	UFUNCTION()
	void OnButton_SearchSessionClicked();
	
	UFUNCTION()
	void OnButton_QuitGameClicked();

	void OnCreateAndStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type bWasSuccessful, const FString& Address);

	UPROPERTY(Transient)
	UOnlineSessionSubsystem* OnlineSessionSubsystem;
};
