// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "Interfaces/OnlineSessionDelegates.h"
#include "NRLobbyUserWidget.generated.h"

class UButton;
class UBorder;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
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
	
protected:
	virtual void BindWidgetEvent() override;

	//~Begin This Class
private:
	UFUNCTION()
	void OnButton_NewGameClicked();

	UFUNCTION()
	void OnButton_SearchSessionClicked();
	
	UFUNCTION()
	void OnButton_QuitGameClicked();

	UFUNCTION()
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	UFUNCTION()
	void OnFindSessionsComplete(bool bWasSuccessful);

	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;
};
