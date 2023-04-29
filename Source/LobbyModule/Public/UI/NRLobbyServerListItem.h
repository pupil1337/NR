// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "NRLobbyServerListItem.generated.h"

class UButton;
class UTextBlock;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class LOBBYMODULE_API UNRLobbyServerListItem : public UNRUserWidgetBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Select;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ServerName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_PlayerNum;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Ping;

//~Begin This Class
public:
	void Update(const FOnlineSessionSearchResult& SearchResult) const;
};
