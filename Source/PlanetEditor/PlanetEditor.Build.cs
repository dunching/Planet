// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class PlanetEditor : ModuleRules
{
	public PlanetEditor(ReadOnlyTargetRules Target) : base(Target)
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

		PublicIncludePaths.Add("PlanetEditor");
		PublicIncludePaths.Add("PlanetEditor/Command");

		PrivateIncludePaths.Add("PlanetEditor/Private");

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
			"Tools",
			"Gravity",
			"Utils",
			"Planet",

			// 其他模块
			"Tools",
			"GameplayTagsLibrary",
			"CommonType",
			"AssetRef",
			"GameOptions",
		});

		PublicDependencyModuleNames.AddRange(new string[]
		{
		});

		// b*[^:b#/]+.*$
	}
}