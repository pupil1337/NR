// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Interact/NRInteractWeaponUserWidget.h"

void UNRInteractWeaponUserWidget::OnLoseWeapon()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UNRInteractWeaponUserWidget::OnFindWeapon()
{
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);	
}
