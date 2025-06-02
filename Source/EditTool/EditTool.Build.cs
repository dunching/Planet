// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EditTool : ModuleRules
{
	public EditTool(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		UndefinedIdentifierWarningLevel = WarningLevel.Error;
		bWarningsAsErrors = true;
		bEnableExceptions = true;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
		}

		CppStandard = CppStandardVersion.Cpp20;
		bUseRTTI = true;

		PrivateDependencyModuleNames.AddRange(new string[] { });

		PublicIncludePaths.Add("EditTool");
		PublicIncludePaths.Add("EditTool/Command");

		PrivateIncludePaths.Add("EditTool/Private");

		if (Target.bBuildEditor == true)
		{
			PrivateDependencyModuleNames.Add("UnrealEd");
		}

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			// 引擎内容
			"Core",
			"CoreUObject",
			"Engine",
			"GameplayTags",

			// 引擎插件
			"NavigationSystem",
			"AIModule",
			"GameplayAbilities",

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
			"GroupManagger",
			
			// 其他模块
			"AssetRef",
			"ChallengeSystem",
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

		PublicDependencyModuleNames.AddRange(new string[]
		{
		});

		PrivateIncludePathModuleNames.AddRange(new string[]
		{
		});
		
		PublicIncludePathModuleNames.AddRange(new string[]
		{
		});
	}
}