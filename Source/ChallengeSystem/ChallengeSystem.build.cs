// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ChallengeSystem : ModuleRules
{
	public ChallengeSystem(ReadOnlyTargetRules Target) : base(Target)
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

        PublicIncludePaths.Add("ChallengeSystem/ChallengeSystem");

        PrivateDependencyModuleNames.AddRange(new string[] {
            // 引擎内容
            "Core",
            "CoreUObject",
            "Engine",
            "GameplayTags",
            "InputCore",
            "GameplayTags",
            "GameplayAbilities",
            "EnhancedInput",
            "ActorSequence", 
            "UMG", 

			// 引擎插件
			"GameplayStateTreeModule",
			"NavigationSystem",
			"AIModule",
			"GameplayAbilities",
			"Niagara",

            // 插件
			"Common_UMG",
			"CommonUser", 
			"Gravity",
			"GuideSystem",
			"StateProcessor",
			"Utils",
			"Weather",
			"NetAbilitySystem",
			"GameplayEffectDataModify",
			"ItemProxy",
			"SceneActor",
			"GroupManagger",
			"NPCGenerator",
			"Utils",
			
			// 其他模块
			"AssetRef",
			"CommonType",
			"GameOptions",
			"GameplayTagsLibrary",
			
			"Planet",
			
			"Planet_GroupManagger",
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
