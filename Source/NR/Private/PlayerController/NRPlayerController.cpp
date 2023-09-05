// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/NRPlayerController.h"

#include "Character/NRCharacter.h"

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

void ANRPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);

	if (ANRCharacter* NRCharacter = Cast<ANRCharacter>(P))
	{
		if (UAbilitySystemComponent* ASC = NRCharacter->GetAbilitySystemComponent())
		{
			ASC->InitAbilityActorInfo(NRCharacter, NRCharacter);
		}
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

