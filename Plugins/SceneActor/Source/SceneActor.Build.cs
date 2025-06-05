// Copyright 2019 Tefel. All Rights Reserved

using System.IO;
using UnrealBuildTool;

public class SceneActor : ModuleRules
{
	public SceneActor(ReadOnlyTargetRules Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.Latest;

        CppStandard = CppStandardVersion.Cpp20;

        PublicIncludePaths.AddRange(
			new string[] {
                   Path.Combine(PluginDirectory, "Source"),
                   Path.Combine(PluginDirectory, "Source/SceneActor"),
                   Path.Combine(PluginDirectory, "Source/SceneActor"),
                   Path.Combine(PluginDirectory, "Source/SceneActor/Consumables"),
                   Path.Combine(PluginDirectory, "Source/SceneActor/Equipments"),
                   Path.Combine(PluginDirectory, "Source/SceneActor/Equipments/Tools"),
                   Path.Combine(PluginDirectory, "Source/SceneActor/Equipments/Weapons"),
            }
			);
				
		PrivateIncludePaths.AddRange(
			new string[] {
            }
			);

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }

        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
            }
			);

		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
                "GameplayTags",
                "Json"
            }
			);

        DynamicallyLoadedModuleNames.AddRange(
			new string[]
            {
            }
			);
	}
}
