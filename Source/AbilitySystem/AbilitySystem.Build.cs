// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class AbilitySystem : ModuleRules
{
	public AbilitySystem(ReadOnlyTargetRules Target) : base(Target)
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

        PublicIncludePaths.Add("AbilitySystem");
        PublicIncludePaths.Add("AbilitySystem/Helper_RootMotionSource");
        PublicIncludePaths.Add("AbilitySystem/Helper_Tasks");
        PublicIncludePaths.Add("AbilitySystem/Helper_Async");
        PublicIncludePaths.Add("AbilitySystem/BasicFutures");
        PublicIncludePaths.Add("AbilitySystem/Communication");
        PublicIncludePaths.Add("AbilitySystem/Skills");
        PublicIncludePaths.Add("AbilitySystem/Skills/Active");
        PublicIncludePaths.Add("AbilitySystem/Skills/Passive");
        PublicIncludePaths.Add("AbilitySystem/Skills/Weapon");
        PublicIncludePaths.Add("AbilitySystem/Skills/Talent");
        PublicIncludePaths.Add("AbilitySystem/Skills/Element");
        PublicIncludePaths.Add("AbilitySystem/ToolFutures");
        PublicIncludePaths.Add("AbilitySystem/SPlineActor");
        PublicIncludePaths.Add("AbilitySystem/ToolsInteractiveComponent");

        PrivateDependencyModuleNames.AddRange(new string[] { });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "GameplayTasks",
            "Niagara",

            "SceneObjects",
            "Tools",
        });

        PublicDependencyModuleNames.AddRange(new string[] {
            "GameplayAbilities",
        });

        // b*[^:b#/]+.*$
    }
}
