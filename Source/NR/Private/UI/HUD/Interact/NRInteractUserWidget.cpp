// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Interact/NRInteractUserWidget.h"

#include "Interface/NRInteractInterface.h"
#include "UI/HUD/Interact/NRInteractMonsterLifeUserWidget.h"

void UNRInteractUserWidget::OnLoseInteraction(AActor* InActor)
{
	ensureAlwaysMsgf(CurrInteraction.Get() != nullptr, TEXT("Lose Interaction, But: CurrInteraction == nullptr"));
	ensureAlwaysMsgf(InActor == CurrInteraction.Get(), TEXT("Lose Interaction, But: InActor != CurrInteraction"));

	if (CurrInteraction.Get())
	{
		DoInteract(Lose);
	}
	CurrInteraction.Reset();
}

void UNRInteractUserWidget::OnFindInteraction(AActor* InActor)
{
	ensureAlwaysMsgf(CurrInteraction.Get() == nullptr, TEXT("Find Interaction, But: CurrInteraction != nullptr"));
	ensureAlwaysMsgf(InActor != nullptr,               TEXT("Find Interaction, But: InActor == nullptr"));

	if (CurrInteraction.Get())
	{
		DoInteract(Lose);
	}
	
	CurrInteraction = InActor;
	DoInteract(Find);
}

void UNRInteractUserWidget::DoInteract(EInteractViewOp ViewOp)
{
	if (CurrInteraction.Get()->Implements<UNRInteractInterface>())
	{
		switch (Cast<INRInteractInterface>(CurrInteraction.Get())->GetInteractionType())
		{
			case ENRInteractionType::EIT_Weapon:
				{
					break;
				}
			case ENRInteractionType::EIT_Monster:
				{
					if (ViewOp == Lose) OnLoseMonster();
					if (ViewOp == Find) OnFindMonster();
					break;
				}
			default: ;
		}
	}
}

void UNRInteractUserWidget::OnLoseMonster()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Silver, FString::Printf(TEXT("Interact LoseMonster: %s"), *CurrInteraction.Get()->GetName()));

	InteractMonsterLifeUserWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UNRInteractUserWidget::OnFindMonster()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString::Printf(TEXT("Interact FindMonster: %s"), *CurrInteraction.Get()->GetName()));

	InteractMonsterLifeUserWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

