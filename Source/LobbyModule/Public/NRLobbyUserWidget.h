﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "NRLobbyUserWidget.generated.h"

class UButton;

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
	TObjectPtr<UButton> Button_Setting;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_QuitGame;
	
protected:
	virtual void BindWidgetEvent() override;

	//~Begin This Class
private:
	UFUNCTION()
	void OnButton_NewGameClicked();
	
	UFUNCTION()
	void OnButton_QuitGameClicked();
};
