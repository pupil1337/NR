// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Menu/NRSettingUserWidget.h"

#include "Components/Button.h"
#include "Subsystem/SaveGame/NRSaveGameSubsystem.h"

void UNRSettingUserWidget::BindWidgetEvent()
{
	Super::BindWidgetEvent();

	if (Button_SaveGame)
	{
		Button_SaveGame->OnClicked.AddUniqueDynamic(this, &ThisClass::OnButton_SaveGameClicked);
	}	
}

void UNRSettingUserWidget::OnButton_SaveGameClicked()
{
	if (const UGameInstance* GameInstance = GetGameInstance())
	{
		if (const UNRSaveGameSubsystem* NRSaveGameSubsystem = GameInstance->GetSubsystem<UNRSaveGameSubsystem>())
		{
			NRSaveGameSubsystem->SaveGame(FAsyncSaveGameToSlotDelegate::CreateLambda([](const FString& SlotName, const int32 UserIndex, bool bSuccess)->void
			{
				UE_LOG(LogTemp, Log, TEXT("SaveGame:: SlotName:%s UserIndex:%d, bSuccess:%d"), *SlotName, UserIndex, bSuccess);
			})
			);
		}
	}
}
