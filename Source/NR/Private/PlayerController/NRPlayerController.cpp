// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/NRPlayerController.h"

#include "NRGameSingleton.h"
#include "NRUserWidgetBase.h"
#include "UI/NRUIUserWidget.h"

#ifdef IMGUI_API
#include "ImGuiModule.h"
#include "DebugConsole/NRDebugConsole.h"
#endif


void ANRPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController())
	{
		const FInputModeGameOnly InputModeGameOnly;
		SetInputMode(InputModeGameOnly);
		SetShowMouseCursor(false);

		CreateUIUserWidget();

#ifdef IMGUI_API
		DebugConsole = MakeShareable(new FNRDebugConsole(this));
		ImGuiDelegateHandle = FImGuiModule::Get().AddWorldImGuiDelegate(FImGuiDelegate::CreateRaw(DebugConsole.Get(), &FNRDebugConsole::Tick));
#endif
	}
}

void ANRPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (IsLocalPlayerController())
	{
#ifdef IMGUI_API
		FImGuiModule::Get().RemoveImGuiDelegate(ImGuiDelegateHandle);
		DebugConsole.Reset();
#endif	
	}
	
	Super::EndPlay(EndPlayReason);
}

void ANRPlayerController::CreateUIUserWidget()
{
	if (!UIUserWidget)
	{
		if (const UNRGameSingleton* NRGameSingleton = UNRGameSingleton::Get())
		{
			if (NRGameSingleton->UIWidgetClass)
			{
				UIUserWidget = CreateWidget<UNRUIUserWidget>(this, NRGameSingleton->UIWidgetClass);
				UIUserWidget->AddToViewport();
			}
		}
	}
}

