// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Tools : ModuleRules
{
	public Tools(ReadOnlyTargetRules Target) : base(Target)
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
        PublicIncludePaths.Add("Tools");
        PublicIncludePaths.Add("Tools/ThreadPoolHelper");
        PublicIncludePaths.Add("Tools/LogHelper");

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
        });

        PublicDependencyModuleNames.AddRange(new string[] {
        });

        // b*[^:b#/]+.*$
    }
}
