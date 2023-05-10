// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "NRGameInstance.generated.h"

class UNRUserWidgetBase;

/**
 * 
 */
UCLASS()
class BASEMODULE_API UNRGameInstance : public UGameInstance
{
	GENERATED_BODY()

//~Begin This Class
public:
	UPROPERTY()
	UNRUserWidgetBase* LobbyWidget;
};
