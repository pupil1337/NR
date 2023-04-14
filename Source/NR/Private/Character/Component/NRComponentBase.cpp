// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Component/NRComponentBase.h"

#include "Character/NRCharacter.h"

UNRComponentBase::UNRComponentBase()
{
	bAutoActivate = true;
	bWantsInitializeComponent = true;
}

void UNRComponentBase::InitializeComponent()
{
	Super::InitializeComponent();

	NRCharacter = Cast<ANRCharacter>(GetOwner());
}
