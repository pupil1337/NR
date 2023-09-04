// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#ifdef IMGUI_API
#include "ImGuiDelegates.h"
#include "DebugConsole/NRDebugConsole.h"
#endif
#include "NRPlayerController.generated.h"

class UNRUIUserWidget;
struct FGameplayAbilityTargetDataHandle;

UCLASS()
class NR_API ANRPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

//~ Begin This Class

	// ----------------------------------------------------------------------------------------------------------------
	// UI Begin
	// ----------------------------------------------------------------------------------------------------------------
public:
	UFUNCTION()
	void OnLoseInteraction(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnFindInteraction(const FGameplayAbilityTargetDataHandle& Data);
	
private:
	void CreateUIUserWidget();
	
	UPROPERTY(Transient)
	UNRUIUserWidget* UIUserWidget;

	// ----------------------------------------------------------------------------------------------------------------
	// UI End
	// ----------------------------------------------------------------------------------------------------------------

#ifdef IMGUI_API
public:
	TSharedPtr<FNRDebugConsole> DebugConsole;
	FImGuiDelegateHandle ImGuiDelegateHandle;
#endif
};
