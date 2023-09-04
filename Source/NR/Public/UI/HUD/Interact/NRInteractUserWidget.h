// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "NRInteractUserWidget.generated.h"


class UNRInteractMonsterLifeUserWidget;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRInteractUserWidget : public UNRUserWidgetBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UNRInteractMonsterLifeUserWidget> InteractMonsterLifeUserWidget;
	
//~Begin This Class
public:
	void OnLoseInteraction(AActor* InActor);

	void OnFindInteraction(AActor* InActor);

private:
	enum EInteractViewOp : uint8
	{
		Lose = 0,
		Find = 1
	};
	void DoInteract(EInteractViewOp ViewOp);
	
	// -----------------------------------------------------------------------------------------------------------------
	// Monster Begin
	// -----------------------------------------------------------------------------------------------------------------

	void OnLoseMonster();

	void OnFindMonster();
	
	// -----------------------------------------------------------------------------------------------------------------
	// Monster End
	// -----------------------------------------------------------------------------------------------------------------
	
private:
	TWeakObjectPtr<AActor> CurrInteraction;
};
