// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Planet_GuideSystem : ModuleRules
{
	public Planet_GuideSystem(ReadOnlyTargetRules Target) : base(Target)
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

        PublicIncludePaths.Add("Planet_GuideSystem/ChallengeSystem");
        PublicIncludePaths.Add("Planet_GuideSystem/ChallengeSystem/GuideActor");
        PublicIncludePaths.Add("Planet_GuideSystem/ChallengeSystem/STE");
        PublicIncludePaths.Add("Planet_GuideSystem/ChallengeSystem/STT");
        PublicIncludePaths.Add("Planet_GuideSystem/GuideSystem");
        PublicIncludePaths.Add("Planet_GuideSystem/GuideSystem/GuideActor");
        PublicIncludePaths.Add("Planet_GuideSystem/GuideSystem/GuideActor/GuideInteraction");
        PublicIncludePaths.Add("Planet_GuideSystem/GuideSystem/GuideActor/GuideThread");
        PublicIncludePaths.Add("Planet_GuideSystem/GuideSystem/STT");
        PublicIncludePaths.Add("Planet_GuideSystem/GuideSystem/STT/GuideInteraction");
        PublicIncludePaths.Add("Planet_GuideSystem/GuideSystem/STT/GuideThread");
        PublicIncludePaths.Add("Planet_GuideSystem/GuideSystem/STE");
        PublicIncludePaths.Add("Planet_GuideSystem/GuideSystem/STE/GuideInteraction");
        PublicIncludePaths.Add("Planet_GuideSystem/GuideSystem/STE/GuideThread");
        PublicIncludePaths.Add("Planet_GuideSystem/GuideSystem/GuideSystem_GameplayTask");
        PublicIncludePaths.Add("Planet_GuideSystem/GuideSystem/GuideSystem_GameplayTask/GuideThread");
        PublicIncludePaths.Add("Planet_GuideSystem/GuideSystem/GuideSystem_GameplayTask/GuideInteraction");

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
            "StateTreeModule",

			// 引擎插件
			"GameplayTasks",
			"NavigationSystem",
			"AIModule",
			"GameplayAbilities",
			"GameplayStateTreeModule",

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
            "Planet_GroupManagger",
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
