// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Interact/NRInteractUserWidget.h"

#include "AbilitySystemComponent.h"
#include "AI/NRMonster.h"
#include "AI/GAS/NRAS_Monster.h"
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
}

void UNRInteractUserWidget::OnLoseMonster()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Silver, FString::Printf(TEXT("Interact LoseMonster: %s"), *CurrInteraction.Get()->GetName()));

	InteractMonsterLifeUserWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UNRInteractUserWidget::OnFindMonster()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::White, FString::Printf(TEXT("Interact FindMonster: %s"), *CurrInteraction.Get()->GetName()));

	InteractMonsterLifeUserWidget->OnFindMonster(Cast<ANRMonster>(CurrInteraction.Get()));
	
	// if (ANRMonster* Monster = Cast<ANRMonster>(CurrInteraction.Get()))
	// {
	// 	if (UAbilitySystemComponent* ASC = Monster->GetAbilitySystemComponent())
	// 	{
	// 		if (const UNRAS_Monster* AS_Monster = Cast<UNRAS_Monster>(ASC->GetAttributeSet(UNRAS_Monster::StaticClass())))
	// 		{
	// 			// ASC->GetGameplayAttributeValueChangeDelegate(AS_Monster->GetHealth)
	// 		}
	// 	}
	// }
}

