// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ShiYu : ModuleRules
{
	public ShiYu(ReadOnlyTargetRules Target) : base(Target)
    {
        bEnableUndefinedIdentifierWarnings = false;

        bEnableExceptions = true;

        DefaultBuildSettings = BuildSettingsVersion.V2;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
        }

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }

        CppStandard = CppStandardVersion.Cpp20;
        bUseRTTI = true;

        PublicDefinitions.Add("TESTHOLDDATA = 1");

        PrivateDependencyModuleNames.AddRange(new string[] { });
        PublicIncludePaths.Add("ShiYu");
        PublicIncludePaths.Add("ShiYu/Common");
        PublicIncludePaths.Add("ShiYu/Library");
        PublicIncludePaths.Add("ShiYu/GamePlay");
        PublicIncludePaths.Add("ShiYu/GamePlay/Animation");
        PublicIncludePaths.Add("ShiYu/GamePlay/Animation/Horse");
        PublicIncludePaths.Add("ShiYu/GamePlay/Animation/Human");
        PublicIncludePaths.Add("ShiYu/GamePlay/SceneObj");
        PublicIncludePaths.Add("ShiYu/GamePlay/SceneObj/Biomes");
        PublicIncludePaths.Add("ShiYu/GamePlay/Pawn");
        PublicIncludePaths.Add("ShiYu/GamePlay/Pawn/Horse");
        PublicIncludePaths.Add("ShiYu/GamePlay/Pawn/Horse");
        PublicIncludePaths.Add("ShiYu/GamePlay/Pawn/Horse/ActionProcess");
        PublicIncludePaths.Add("ShiYu/GamePlay/Pawn/Human");
        PublicIncludePaths.Add("ShiYu/GamePlay/Pawn/Human");
        PublicIncludePaths.Add("ShiYu/GamePlay/Pawn/Human/ActionProcess");
        PublicIncludePaths.Add("ShiYu/GamePlay/Pawn/Human/ActionProcess/ActionPlace");
        PublicIncludePaths.Add("ShiYu/Tools");

        PrivateDependencyModuleNames.AddRange(new string[] {
            "VoxelCore",
            "VoxelGraphCore",
            "Gravity",
            "MMT",
            "EnhancedInput" ,
        });

        PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine", 
			"InputCore",
			"SlateCore", 
			"Niagara", 
			"UMG", 
			"AIModule",
			"NavigationSystem",
            "ChaosVehicles",
        });
    }
}
