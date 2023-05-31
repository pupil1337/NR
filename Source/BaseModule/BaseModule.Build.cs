using UnrealBuildTool;

public class BaseModule : ModuleRules
{
    public BaseModule(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "Core", "CoreUObject", "Engine",
            
            "UMG",
            
            "OnlineSubsystem",
            
            "Niagara"
        });
    }
}