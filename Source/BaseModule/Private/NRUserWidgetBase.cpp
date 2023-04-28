// Fill out your copyright notice in the Description page of Project Settings.


#include "NRUserWidgetBase.h"

void UNRUserWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BindWidgetEvent();
}

void UNRUserWidgetBase::NativeDestruct()
{
	Super::NativeDestruct();

	UnBindExternalEvent();
}
