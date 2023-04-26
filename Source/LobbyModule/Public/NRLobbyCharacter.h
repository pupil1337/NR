// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseModule/Public/NRCharacterBase.h"
#include "NRLobbyCharacter.generated.h"

UCLASS(Abstract, Blueprintable)
class LOBBYMODULE_API ANRLobbyCharacter : public ANRCharacterBase
{
	GENERATED_BODY()

public:
	ANRLobbyCharacter(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
};
