// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlanetEditor : ModuleRules
{
	public PlanetEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        bEnableUndefinedIdentifierWarnings = false;
        bWarningsAsErrors = true;
        bEnableExceptions = true;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
        }

        CppStandard = CppStandardVersion.Cpp20;
        bUseRTTI = true;

        PrivateDependencyModuleNames.AddRange(new string[] { });
        PublicIncludePaths.Add("PlanetEditor");
        PublicIncludePaths.Add("PlanetEditor/PlanetEditor");
        PublicIncludePaths.Add("PlanetEditor/PlanetEditor/Command");

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
        });

        PublicDependencyModuleNames.AddRange(new string[] {
            "GameplayAbilities",

            "Planet",
        });

        // b*[^:b#/]+.*$
    }
}
