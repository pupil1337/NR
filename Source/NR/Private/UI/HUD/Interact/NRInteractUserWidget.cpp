// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Interact/NRInteractUserWidget.h"

#include "AI/NRMonster.h"
#include "Interface/NRInteractInterface.h"
#include "UI/HUD/Interact/NRInteractMonsterLifeUserWidget.h"

void UNRInteractUserWidget::OnLoseInteraction(AActor* InActor)
{
	ensureAlwaysMsgf(CurrInteraction.IsValid(), TEXT("Lose Interaction, But: CurrInteraction == nullptr"));
	ensureAlwaysMsgf(InActor == CurrInteraction.Get(), TEXT("Lose Interaction, But: InActor != CurrInteraction"));

	DoInteract(Lose);
	CurrInteraction.Reset();
}

void UNRInteractUserWidget::OnFindInteraction(AActor* InActor)
{
	ensureAlwaysMsgf(!CurrInteraction.IsValid(), TEXT("Find Interaction, But: CurrInteraction != nullptr"));
	ensureAlwaysMsgf(InActor != nullptr,               TEXT("Find Interaction, But: InActor == nullptr"));

	DoInteract(Lose);
	CurrInteraction = InActor;
	DoInteract(Find);
}

void UNRInteractUserWidget::DoInteract(EInteractViewOp ViewOp)
{
	if (CurrInteraction.IsValid())
	{
		if (CurrInteraction.Get()->Implements<UNRInteractInterface>())
		{
			const ENRInteractionType InteractionType = Cast<INRInteractInterface>(CurrInteraction.Get())->GetInteractionType();
			if (ViewOp == Find) CloseOtherInteract(InteractionType);
			
			switch (InteractionType)
			{
			case ENRInteractionType::EIT_Weapon:
				{
					break;
				}
			case ENRInteractionType::EIT_Monster:
				{
					if (ViewOp == Lose) OnLoseMonster(true);
					if (ViewOp == Find) OnFindMonster();
					break;
				}
			default: ;
			}
		}	
	}
}

void UNRInteractUserWidget::CloseOtherInteract(ENRInteractionType CurrType)
{
	if (CurrType != ENRInteractionType::EIT_Monster) OnLoseMonster(false);
}

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

