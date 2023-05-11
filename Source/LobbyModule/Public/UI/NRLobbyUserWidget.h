// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "NRLobbyUserWidget.generated.h"

class UWidgetSwitcher;
class UButton;
class UBorder;
class UScrollBox;
class UTextBlock;
class UNRLobbyServerListItem;
class UOnlineSessionSubsystem;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class LOBBYMODULE_API UNRLobbyUserWidget : public UNRUserWidgetBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher_Panel;
	
	/* Panel 0 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_StartGame;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Setting;// TODO
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_QuitGame;

	/* Panel 1 */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_ContinueGame;// TODO
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_NewGame;// TODO

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_ChangePlayer;// TODO

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_SearchPublic;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_BackTo0;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border_ServerList;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_ServerList;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_CloseServerList;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_FindSession;
	
	// Settings
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UNRLobbyServerListItem> NRLobbyServerListItemClass;
	
protected:
	virtual void BindWidgetEvent() override;
	virtual void UnBindExternalEvent() override;

	//~Begin This Class
private:
	UFUNCTION()
	void OnButton_StartGameClicked();
	
	UFUNCTION()
	void OnButton_NewGameClicked();

	UFUNCTION()
	void OnButton_SearchSessionClicked();
	
	UFUNCTION()
	void OnButton_QuitGameClicked();

	UFUNCTION()
	void OnButton_CloseServerListClicked();

	void OnCreateAndStartSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(const TArray<FOnlineSessionSearchResult>& SearchResults, bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type bWasSuccessful, const FString& Address);

	UPROPERTY(Transient)
	UOnlineSessionSubsystem* OnlineSessionSubsystem;
};
