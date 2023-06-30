// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugConsole/NRDebugConsole.h"

#include "imgui.h"
#include "ImGuiModule.h"
#include "GameFramework/PlayerState.h"
#include "PlayerController/NRPlayerController.h"


NRDebugConsole::NRDebugConsole(ANRPlayerController* NRPlayerController):
	NRPlayerController(NRPlayerController)
{
	GameViewportClient = NRPlayerController->GetLocalPlayer()->ViewportClient;

	NRPlayerController->ConsoleCommand("NetEmulation.PktLag 0");
	NRPlayerController->ConsoleCommand("t.MaxFPS -1"); MaxFPS = -1;
}

NRDebugConsole::~NRDebugConsole()
{
}

void NRDebugConsole::Tick()
{
	ImGuiWindowFlags WindowFlags = 0;
	WindowFlags |= ImGuiWindowFlags_MenuBar;
	WindowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
	if (ImGui::Begin("GM", nullptr, WindowFlags))
	{
		ConsoleSetting();
	
		ConsoleVar();

		ImGui::Dummy({0.0f, 20.0f});
		ImGui::Separator();
		ImGui::Text("Ping: %d ms", NRPlayerController->GetPlayerState<APlayerState>() ? static_cast<int>(NRPlayerController->GetPlayerState<APlayerState>()->GetPingInMilliseconds()) : 999);
		ImGui::SameLine(); ImGui::Text(" %s", NRPlayerController->HasAuthority() ? "[Server]" : "[Client]");
		
		ImGui::End();
	}
}

void NRDebugConsole::ConsoleSetting()
{
	// Auto Expand/Collapse
	ImGui::SetWindowCollapsed(FImGuiModule::Get().IsInputMode() ? false : bAutoCollapse);
    
    // Menu Bar
    if (ImGui::BeginMenuBar())
    {
    	// MenuBar/Options
    	if (ImGui::BeginMenu("Options"))
    	{
    		// MenuBar/Options/Auto Collapse
    		ImGui::Checkbox("Auto Collapse", &bAutoCollapse);
    
    		ImGui::EndMenu();
    	}
    
    	ImGui::EndMenuBar();
    }
}

void NRDebugConsole::ConsoleVar()
{
	if (ImGui::CollapsingHeader("Console Var"))
	{
		ImGui::Indent();
		// Stat FPS
		ImGui::Checkbox("Stat FPS", &bStatFPS);
		if (bStatFPS != GameViewportClient->IsStatEnabled("FPS"))
		{
			TArray<FString> Stats = *GameViewportClient->GetEnabledStats();
			if (Stats.Contains("FPS")) Stats.Remove("FPS");
			else Stats.Push("FPS");
		
			GameViewportClient->SetEnabledStats(Stats);
		}

		// Net Show Corrections
		if (ImGui::Checkbox("p.NetShowCorrections", &bNetShowCorrections))
		{
			NRPlayerController->ConsoleCommand(FString::Printf(TEXT("p.NetShowCorrections %d"), bNetShowCorrections));	
		}

		// MaxFPS
		if (ImGui::SliderInt("t.MaxFPS", &MaxFPS, 30, 999))
		{
			NRPlayerController->ConsoleCommand(FString::Printf(TEXT("t.MaxFPS %d"), MaxFPS));
		}
		
		ImGui::SeparatorText("");
		
		// Show Collision
		ImGui::Checkbox("show Collision", &bShowCollision);
		if (bShowCollision != GameViewportClient->EngineShowFlags.Collision)
		{
			NRPlayerController->ConsoleCommand("show Collision");
		}

		// Show Debug XX
		if (ImGui::TreeNode("Show Debug XX"))
		{
			// ShowDebug EnhancedInput
			if (ImGui::CheckboxFlags("EnhancedInput", &ShowDebugFlags, ShowDebugFlags_EnhancedInput))
			{
				NRPlayerController->ConsoleCommand("ShowDebug EnhancedInput");
				if (ShowDebugFlags == ShowDebugFlags_None) NRPlayerController->ConsoleCommand("ShowDebug");
			}

			// ShowDebug AbilitySystem
			if (ImGui::CheckboxFlags("AbilitySystem", &ShowDebugFlags, ShowDebugFlags_AbilitySystem))
			{
				NRPlayerController->ConsoleCommand("ShowDebug AbilitySystem");
				if (ShowDebugFlags == ShowDebugFlags_None) NRPlayerController->ConsoleCommand("ShowDebug");
			}
			ImGui::BeginDisabled(!(ShowDebugFlags & ShowDebugFlags_AbilitySystem));
			ImGui::SameLine(); if (ImGui::SmallButton("Next Page")) NRPlayerController->ConsoleCommand("AbilitySystem.Debug.NextCategory");
			ImGui::EndDisabled();

			ImGui::TreePop();
		}
		
		// NetLag
		ImGui::BeginDisabled(NRPlayerController->HasAuthority());
		if (ImGui::SliderInt("NetEmulation.PktLag", &NetLag, 0, 999))
		{
			NRPlayerController->ConsoleCommand(FString::Printf(TEXT("NetEmulation.PktLag %d"), NetLag));
		}
		ImGui::EndDisabled();

		ImGui::Unindent();
	}
}
