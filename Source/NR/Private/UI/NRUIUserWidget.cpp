// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NRUIUserWidget.h"

#include "UI/HUD/Interact/NRInteractUserWidget.h"

void UNRUIUserWidget::OnLoseInteraction(AActor* InActor)
{
	InteractUserWidget->OnLoseInteraction(InActor);
}

void UNRUIUserWidget::OnFindInteraction(AActor* InActor)
{
	InteractUserWidget->OnFindInteraction(InActor);
}
