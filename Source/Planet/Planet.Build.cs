// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Planet : ModuleRules
{
	public Planet(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        bEnableUndefinedIdentifierWarnings = false;

        bEnableExceptions = true;

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
        }

        CppStandard = CppStandardVersion.Cpp20;
        bUseRTTI = true;

        PublicDefinitions.Add("TESTHOLDDATA = 1");

        PrivateDependencyModuleNames.AddRange(new string[] { });
        PublicIncludePaths.Add("Planet");
        PublicIncludePaths.Add("Planet/GamePlay");
        PublicIncludePaths.Add("Planet/GamePlay/AI");
        PublicIncludePaths.Add("Planet/GamePlay/AI/AITask");
        PublicIncludePaths.Add("Planet/GamePlay/AI/EQS");
        PublicIncludePaths.Add("Planet/GamePlay/AI/STE");
        PublicIncludePaths.Add("Planet/GamePlay/AI/STT");
        PublicIncludePaths.Add("Planet/GamePlay/Navgation");
        PublicIncludePaths.Add("Planet/GamePlay/GroupsManagger");
        PublicIncludePaths.Add("Planet/GamePlay/SPlineActor");
        PublicIncludePaths.Add("Planet/GamePlay/UI");
        PublicIncludePaths.Add("Planet/GamePlay/UI/HUD");
        PublicIncludePaths.Add("Planet/GamePlay/UI/HUD/Team");
        PublicIncludePaths.Add("Planet/GamePlay/UI/PawnState");
        PublicIncludePaths.Add("Planet/GamePlay/UI/PawnState/ActionStateHUD");
        PublicIncludePaths.Add("Planet/GamePlay/UI/PawnState/BuildingStateHUD");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Hover");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Hover/DestroyProgress");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Menus");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Menus/GroupManagger");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Menus/Backpacks");
        PublicIncludePaths.Add("Planet/GamePlay/UI/MenuS/CreateMenuS");
        PublicIncludePaths.Add("Planet/GamePlay/UI/MenuS/DragDropOperations");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Menus/SkillsMenu");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Menus/TalentAllocation");
        PublicIncludePaths.Add("Planet/GamePlay/UI/EffectsList");
        PublicIncludePaths.Add("Planet/GamePlay/UI/UMGs");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Helper_RootMotionSource");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Helper_Tasks");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Helper_Async");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/BasicFutures");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Communication");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/ToolFutures");
        PublicIncludePaths.Add("Planet/GamePlay/Animation");
        PublicIncludePaths.Add("Planet/GamePlay/Controller");
        PublicIncludePaths.Add("Planet/GamePlay/GameInstance");
        PublicIncludePaths.Add("Planet/GamePlay/PlayerState");
        PublicIncludePaths.Add("Planet/GamePlay/Animation/Horse");
        PublicIncludePaths.Add("Planet/GamePlay/Animation/Human");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Biomes");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Equipments");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Equipments/Tools");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Equipments/Weapons");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Building");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Projectile");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/InputComponent");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Horse");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Horse");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Horse/ActionProcess");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Human");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Human/ActionProcess");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Human/ActionProcess/ActionProcessor");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Human/ActionProcess/BuildingProcessor/PlacingBuindingsProcessor");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Human/ActionProcess/BuildingProcessor/UseToolsProcessor");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Human/ActionProcess/BuildingProcessor");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Human/ActionProcess/InteractionProcessor");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Human/ActionProcess/ViewMenusProcessor");
        PublicIncludePaths.Add("Planet/GamePlay/WorldSetting");
        PublicIncludePaths.Add("Planet/Command");
        PublicIncludePaths.Add("Planet/Common");
        PublicIncludePaths.Add("Planet/Library");
        PublicIncludePaths.Add("Planet/Tools");
        PublicIncludePaths.Add("Planet/Tools/ThreadPoolHelper");
        PublicIncludePaths.Add("Planet/Tools/LogHelper");

        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.Add("UnrealEd");
        }

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "SlateCore",
            "Slate",
            "Niagara",
            "UMG",

            "EnhancedInput" ,
            "GameplayAbilities",
            "GameplayTags",
            "GameplayTasks",

            "GameplayStateTreeModule",
            "AIModule",
            "StateTreeModule",
            "NavigationSystem",
            "ChaosVehicles",
        });

        PublicDependencyModuleNames.AddRange(new string[] {
            "VoxelCore",
            "VoxelGraphCore",
            "Gravity",
            "MMT",
            "FlyingNavSystem" ,
        });

        // b*[^:b#/]+.*$
    }
}
