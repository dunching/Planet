// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class EditTool : ModuleRules
{
	public EditTool(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		CppCompileWarningSettings.UndefinedIdentifierWarningLevel = WarningLevel.Error;
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
			"Json",

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
			"ItemProxy",
			"NetAbilitySystem",
			"NPCGenerator",
			"PropertyEntrySystem",
			"QuestSystem",
			"SceneActor",
			"StateProcessor",
			"Utils",
			"Weather",
			
			// 其他模块
			"AssetRef",
			"CommonType",
			"GameOptions",
			"GameplayTagsLibrary",
			"Planet",
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