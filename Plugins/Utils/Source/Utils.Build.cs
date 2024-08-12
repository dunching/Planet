// Copyright 2019 Tefel. All Rights Reserved

using System.IO;
using UnrealBuildTool;

public class Utils : ModuleRules
{
	public Utils(ReadOnlyTargetRules Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.Latest;

        CppStandard = CppStandardVersion.Cpp20;

        PublicIncludePaths.AddRange(
			new string[] {
                   Path.Combine(PluginDirectory, "Source/Utils"),
                   Path.Combine(PluginDirectory, "Source/Utils/LogHelper"),
                   Path.Combine(PluginDirectory, "Source/Utils/ThreadPoolHelper"),
                   Path.Combine(PluginDirectory, "Source/Utils/BaseData"),
                   Path.Combine(PluginDirectory, "Source/Utils/ScopeEvent"),
            }
			);
				
		PrivateIncludePaths.AddRange(
			new string[] {
                   Path.Combine(PluginDirectory, "Source/Command"),
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
            }
			);

        DynamicallyLoadedModuleNames.AddRange(
			new string[]
            {
            }
			);
	}
}
