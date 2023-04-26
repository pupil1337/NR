// Fill out your copyright notice in the Description page of Project Settings.

using UnrealBuildTool;

public class NR : ModuleRules
{
	public NR(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", "CoreUObject", "Engine",
			
			"EnhancedInput",
			"UMG",
		});
	}
}
