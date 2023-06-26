// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class ANRPlayerController;

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

	bool bStatFPS = true;
	bool bTest = false;
	
	ANRPlayerController* NRPlayerController;
};
