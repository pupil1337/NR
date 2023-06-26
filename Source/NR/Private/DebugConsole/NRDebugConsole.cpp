// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugConsole/NRDebugConsole.h"

#include "imgui.h"
#include "PlayerController/NRPlayerController.h"


NRDebugConsole::NRDebugConsole(ANRPlayerController* NRPlayerController):
	NRPlayerController(NRPlayerController)
{
}

NRDebugConsole::~NRDebugConsole()
{
}

void NRDebugConsole::Tick()
{
	ImGui::Checkbox("Test", &bTest);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
}
