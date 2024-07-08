// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SceneObjects : ModuleRules
{
	public SceneObjects(ReadOnlyTargetRules Target) : base(Target)
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

        PrivateIncludePaths.Add("SceneObjects/Private");

        PublicIncludePaths.Add("SceneObjects");
        PublicIncludePaths.Add("SceneObjects/Common");
        PublicIncludePaths.Add("SceneObjects/SceneTools");
        PublicIncludePaths.Add("SceneObjects/SceneTools/Biomes");
        PublicIncludePaths.Add("SceneObjects/SceneTools/Equipments");
        PublicIncludePaths.Add("SceneObjects/SceneTools/Equipments/Tools");
        PublicIncludePaths.Add("SceneObjects/SceneTools/Equipments/Weapons");
        PublicIncludePaths.Add("SceneObjects/SceneTools/Building");
        PublicIncludePaths.Add("SceneObjects/SceneTools/Building/BuildingArea");
        PublicIncludePaths.Add("SceneObjects/SceneTools/Projectile");

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "Niagara",

            "AbilitySystem",
            "Tools",
        });

        PublicDependencyModuleNames.AddRange(new string[] {
        });

        // b*[^:b#/]+.*$
    }
}
