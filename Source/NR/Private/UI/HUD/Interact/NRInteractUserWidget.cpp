// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Interact/NRInteractUserWidget.h"

#include "AI/NRMonster.h"
#include "Interface/NRInteractInterface.h"
#include "UI/HUD/Interact/NRInteractMonsterLifeUserWidget.h"
#include "UI/HUD/Interact/NRInteractWeaponUserWidget.h"
#include "Actor/Weapon/NRWeaponBase.h"

void UNRInteractUserWidget::OnFindInteraction(AActor* NewInteraction)
{
	if (CurrInteraction.Get() != NewInteraction)
	{
		// 旧的失去视野
		if (CurrInteraction.IsValid() && CurrInteraction->Implements<UNRInteractInterface>())
		{
			DoViewOp(Cast<INRInteractInterface>(CurrInteraction.Get())->GetInteractionType(), Lose);
		}

		// 新的进入视野
		CurrInteraction = NewInteraction;
		if (CurrInteraction.IsValid() && CurrInteraction->Implements<UNRInteractInterface>())
		{
			DoViewOp(Cast<INRInteractInterface>(CurrInteraction.Get())->GetInteractionType(), Find);
		}
	}
}

void UNRInteractUserWidget::DoViewOp(ENRInteractionType Type, EInteractViewOp ViewOp)
{
	if (ViewOp == Find) CloseOtherInteractImmediately(Type);
	
	switch (Type)
	{
		case ENRInteractionType::EIT_Weapon:
		{
			if (ViewOp == Lose) OnLoseWeapon();
			if (ViewOp == Find) OnFindWeapon();
			break;
		}
		case ENRInteractionType::EIT_Monster:
		{
			if (ViewOp == Lose) OnLoseMonster(true);
			if (ViewOp == Find) OnFindMonster();
			break;
		}
		default:;
	}
}

void UNRInteractUserWidget::CloseOtherInteractImmediately(ENRInteractionType CurrType)
{
	if (CurrType != ENRInteractionType::EIT_Monster) OnLoseMonster(false);
	if (CurrType != ENRInteractionType::EIT_Weapon) OnLoseWeapon();
}

// Monster -------------------------------------------------------------------------------------------------------------

void UNRInteractUserWidget::OnLoseMonster(bool bDelay)
{
	if (bDelay)
	{
		GetWorld()->GetTimerManager().SetTimer(CloseMonsterHandle, FTimerDelegate::CreateLambda([this]()
		{
			InteractMonsterLifeUserWidget->OnLoseMonster();
		}),
		1.0f, false);	
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(CloseMonsterHandle);
		InteractMonsterLifeUserWidget->OnLoseMonster();
	}
}

void UNRInteractUserWidget::OnFindMonster()
{
	GetWorld()->GetTimerManager().ClearTimer(CloseMonsterHandle);
	InteractMonsterLifeUserWidget->OnFindMonster(Cast<ANRMonster>(CurrInteraction.Get()));
}

// Weapon --------------------------------------------------------------------------------------------------------------

void UNRInteractUserWidget::OnLoseWeapon()
{
	InteractWeaponUserWidget->OnLoseWeapon();
}

void UNRInteractUserWidget::OnFindWeapon()
{
	InteractWeaponUserWidget->OnFindWeapon(Cast<ANRWeaponBase>(CurrInteraction.Get()));
}

