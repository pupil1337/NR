﻿using UnrealBuildTool;

public class LobbyModule : ModuleRules
{
    public LobbyModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine",
            
            "UMG",
            
            "BaseModule"
        });
        
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "OnlineSubsystem"
        });
    }
}