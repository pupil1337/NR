// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRUserWidgetBase.h"
#include "Types/NRInteractionTypes.h"
#include "NRInteractUserWidget.generated.h"

class UNRInteractMonsterLifeUserWidget;
class UNRInteractWeaponUserWidget;

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class NR_API UNRInteractUserWidget : public UNRUserWidgetBase
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UNRInteractMonsterLifeUserWidget> InteractMonsterLifeUserWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UNRInteractWeaponUserWidget> InteractWeaponUserWidget;
	
//~Begin This Class
public:
	void OnFindInteraction(AActor* NewInteraction);

private:
	enum EInteractViewOp : uint8
	{
		Lose = 0,
		Find = 1
	};
	void DoViewOp(ENRInteractionType Type, EInteractViewOp ViewOp);
	
	void CloseOtherInteractImmediately(ENRInteractionType CurrType);
	
	// -----------------------------------------------------------------------------------------------------------------
	// Monster
	// -----------------------------------------------------------------------------------------------------------------

	void OnLoseMonster(bool bDelay);

	void OnFindMonster();

	FTimerHandle CloseMonsterHandle;
	
	// -----------------------------------------------------------------------------------------------------------------
	// Weapon
	// -----------------------------------------------------------------------------------------------------------------

	void OnLoseWeapon();

	void OnFindWeapon();
	
private:
	TWeakObjectPtr<AActor> CurrInteraction;
};
