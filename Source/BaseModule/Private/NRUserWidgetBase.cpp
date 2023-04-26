// Fill out your copyright notice in the Description page of Project Settings.


#include "NRUserWidgetBase.h"

bool UNRUserWidgetBase::Initialize()
{
	if (Super::Initialize())
	{
		BindWidgetEvent();
		return true;
	}
	return false;
}
