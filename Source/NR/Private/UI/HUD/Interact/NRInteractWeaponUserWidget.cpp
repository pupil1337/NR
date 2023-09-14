// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/Interact/NRInteractWeaponUserWidget.h"
#include "Actor/Weapon/NRWeaponBase.h"

void UNRInteractWeaponUserWidget::OnLoseWeapon()
{
	if (Weapon.IsValid())
	{

		Weapon.Reset();
	}
	
	SetVisibility(ESlateVisibility::Collapsed);
}

void UNRInteractWeaponUserWidget::OnFindWeapon(ANRWeaponBase* InWeapon)
{
	if (InWeapon && InWeapon != Weapon.Get())
	{
		OnLoseWeapon();

		Weapon = InWeapon;
		
	}
	
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);	
}
