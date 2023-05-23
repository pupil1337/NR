// Fill out your copyright notice in the Description page of Project Settings.


#include "NRGameSingleton.h"


void UNRGameSingleton::AddSoftObjectPathToArray(const TSoftObjectPtr<UObject>& SoftObjectPtr, TArray<FSoftObjectPath>& PathArray)
{
	if (SoftObjectPtr.IsPending())
	{
		PathArray.Add(SoftObjectPtr.ToSoftObjectPath());
	}
}
