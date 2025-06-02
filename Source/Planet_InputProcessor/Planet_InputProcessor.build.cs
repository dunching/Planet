// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Planet_InputProcessor : ModuleRules
{
	public Planet_InputProcessor(ReadOnlyTargetRules Target) : base(Target)
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

        PublicIncludePaths.Add("Planet_InputProcessor/InputActions");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/TransitionProcessor");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/BirdProcessor");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/HorseProcessor");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/HumanProcessor");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/HumanProcessor/TransactionProcessor");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/HumanProcessor/ActionProcessor");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/HumanProcessor/EndangeredProcessor");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/HumanProcessor/InteractionProcessor");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/HumanProcessor/BuildingProcessor");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/HumanProcessor/BuildingProcessor/PlacingBuindingsProcessor");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/HumanProcessor/BuildingProcessor/UseToolsProcessor");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/HumanProcessor/ViewMenusProcessor");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/VehicleProcessor");
        PublicIncludePaths.Add("Planet_InputProcessor/InputProcessor/VehicleProcessor/4WheeledVehicle");

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
            "Planet_GroupManagger",
            "Planet_GuideSystem",
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
