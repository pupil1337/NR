// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/NRUIUserWidget.h"

#include "UI/HUD/Interact/NRInteractUserWidget.h"

void UNRUIUserWidget::OnFindInteraction(AActor* NewInteraction)
{
	InteractUserWidget->OnFindInteraction(NewInteraction);
}
