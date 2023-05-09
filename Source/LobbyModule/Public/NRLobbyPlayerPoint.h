// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRPointBase.h"
#include "NRLobbyPlayerPoint.generated.h"

UCLASS()
class LOBBYMODULE_API ANRLobbyPlayerPoint : public ANRPointBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	uint32 Index = 0;
};
