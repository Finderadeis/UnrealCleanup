// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UnrealCleanup : ModuleRules
{
	public UnrealCleanup(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
                System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Editor/Blutility/Public",
            }
            );
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
                System.IO.Path.GetFullPath(Target.RelativeEnginePath) + "/Source/Editor/Blutility/Private",
            }
            );
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
                "Core",
                "Blutility",
                "EditorScriptingUtilities",
                "UMG",
                "Niagara",
                "NiagaraEditor",
                "AssetTools",
                "ContentBrowser",
                "InputCore",
                "Projects",
            }
			);
			PublicDependencyModuleNames.Add(Target.bBuildEditor ? "UnrealEd" : "UnrealEngine");
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "Blutility",
				"EditorScriptingUtilities",
				"AssetTools",
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
