// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

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
class NRDebugConsole
{
public:
	explicit NRDebugConsole(ANRPlayerController* NRPlayerController);
	~NRDebugConsole();
	
	void Tick();

private:
	void ConsoleSetting();
	void ConsoleVar();
	
	// console settings
	inline static bool bAutoCollapse = false;

	// 1. Console Var
	inline static bool bStatFPS = true;
	inline static bool bNetShowCorrections = false;
	inline static int MaxFPS = -1;
	bool bShowCollision = false;
	ShowDebugFlags ShowDebugFlags = 0;
	int NetLag = 0;
	
	// temp
	ANRPlayerController* NRPlayerController;
	UGameViewportClient* GameViewportClient;
};
