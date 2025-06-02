// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Planet_GroupManagger : ModuleRules
{
	public Planet_GroupManagger(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        UndefinedIdentifierWarningLevel = WarningLevel.Error;
        bWarningsAsErrors = true;
        bEnableExceptions = true;

        CppStandard = CppStandardVersion.Cpp20;
        
        bUseRTTI = true;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
        }

        PublicIncludePaths.Add("Planet_GroupManagger");
        PublicIncludePaths.Add("Planet_GroupManagger/GroupManagger");
        PublicIncludePaths.Add("Planet_GroupManagger/TeamMatesHelper");

        PrivateDependencyModuleNames.AddRange(new string[] {
            // 引擎内容
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "InputCore",
            "GameplayTags",
            "GameplayAbilities",

			// 引擎插件
			"NavigationSystem",
			"AIModule",
			"GameplayAbilities",

            // 插件
            "Common_UMG",
            "CommonUser",
            "GameplayEffectDataModify",
            "Gravity",
            "GroupManagger",
            "GuideSystem",
            "ItemProxy",
            "NetAbilitySystem",
            "NPCGenerator",
            "SceneActor",
            "StateProcessor",
            "Utils",
            "Weather",
            
            // 其他模块
            "Planet",
            
            "AssetRef",
            "ChallengeSystem",
            "CommonType",
            "GameOptions",
            "GameplayTagsLibrary",
            "Planet_GuideSystem",
            "Planet_InputProcessor",
            "Planet_ItemProxy",
            "Planet_NPCGenerator",
            "Tools",
        });

        PublicDependencyModuleNames.AddRange(new string[] {
        });
		
        PrivateIncludePathModuleNames.AddRange(new string[]
        {
        });
		
        PublicIncludePathModuleNames.AddRange(new string[]
        {
        });
    }
}
