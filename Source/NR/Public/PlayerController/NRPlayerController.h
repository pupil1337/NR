// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NRPlayerController.generated.h"

class UNRUIUserWidget;

UCLASS()
class NR_API ANRPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

//~ Begin This Class
private:
	void CreateUIUserWidget();
	
	UPROPERTY(Transient)
	UNRUIUserWidget* UIUserWidget;
};
