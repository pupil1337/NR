// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugConsole/NRDebugConsole.h"

#include "imgui.h"
#include "ImGuiModule.h"
#include "ImGuiLibrary/Private/imgui_internal.h"
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
	bool bOpen = true;
	ImGuiWindowFlags WindowFlags = 0;
	WindowFlags |= ImGuiWindowFlags_MenuBar;
	ImGui::Begin("GM", &bOpen, WindowFlags);

	static bool bAutoCollapse = true;
	if (!FImGuiModule::Get().IsInputMode()) ImGui::SetWindowCollapsed(bAutoCollapse);
	
	// Menu Bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			// Auto Collapse
			ImGui::Checkbox("Auto Collapse", &bAutoCollapse);

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}
	
	if (ImGui::CollapsingHeader("Console Var"))
	{
		// 1.Stat FPS
		static bool bStatFPS = true;
		ImGui::Checkbox("Stat FPS", &bStatFPS);
		if (UGameViewportClient* GameViewportClient = NRPlayerController->GetLocalPlayer()->ViewportClient; bStatFPS != GameViewportClient->IsStatEnabled("FPS"))
		{
			TArray<FString> Stats = *GameViewportClient->GetEnabledStats();
			if (Stats.Contains("FPS")) Stats.Remove("FPS");
			else Stats.Push("FPS");
		
			GameViewportClient->SetEnabledStats(Stats);
		}
	}
	
	ImGui::End();
}
