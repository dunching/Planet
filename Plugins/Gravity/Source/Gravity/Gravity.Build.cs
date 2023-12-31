// Copyright 2019 Tefel. All Rights Reserved

using System.IO;
using UnrealBuildTool;

public class Gravity : ModuleRules
{
	public Gravity(ReadOnlyTargetRules Target) : base(Target)
    {
        DefaultBuildSettings = BuildSettingsVersion.V2;

        CppStandard = CppStandardVersion.Cpp17;

        PublicIncludePaths.AddRange(
			new string[] {
                   Path.Combine(PluginDirectory, "Source/Gravity"),
                   Path.Combine(PluginDirectory, "Source/Gravity/Character"),
            }
			);
				
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
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
				"Slate",
				"SlateCore",
				"InputCore",
                "ChaosVehicles",
                "ChaosVehiclesCore",
                "ChaosVehiclesEngine",
				"PhysicsCore",
                "MMT",
            }
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
            {
            }
			);
	}
}
