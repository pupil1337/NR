// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NRDebugConsole.generated.h"

class ANRPlayerController;

enum ShowDebugFlags_
{
	ShowDebugFlags_None			 = 0,
	ShowDebugFlags_EnhancedInput = 1 << 0,
	ShowDebugFlags_AbilitySystem = 1 << 1,
};
typedef int ShowDebugFlags;


/**
 * 
 */
USTRUCT()
struct NR_API FNRDebugConsole
{
	GENERATED_BODY()

	FNRDebugConsole();
	explicit FNRDebugConsole(ANRPlayerController* InNRPlayerController);
	
	void Tick();
	
private:
	void ConsoleSetting();
	void ConsoleVar();
	void AbilitySystem();

public:
	// console settings
	inline static bool bAutoCollapse = false;

	// 1. Console Var
	inline static bool bStatFPS = true;
	inline static bool bNetShowCorrections = false;
	inline static int MaxFPS = -1;
	bool bShowCollision = false;
	ShowDebugFlags ShowDebugFlags = 0;
	int NetLag = 0;

	// 2. Ability System
		// · GA_Fire
	bool FireTraceDebug = false;

		// · GA_InteractPassive
	bool InteractTraceDebug = false;
	int InteractAngle = 2;
	int InteractDistance = 10000;

private:
	TWeakObjectPtr<ANRPlayerController> NRPlayerController;
	bool bInitialized = false;
};
