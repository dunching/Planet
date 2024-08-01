// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Planet : ModuleRules
{
	public Planet(ReadOnlyTargetRules Target) : base(Target)
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

        PublicDefinitions.Add("TESTPLAYERCHARACTERHOLDDATA = 0");
        PublicDefinitions.Add("TESTAICHARACTERHOLDDATA = 1");
        PublicDefinitions.Add("TESTRESOURCEBOXHOLDDATA = 1");
        PublicDefinitions.Add("TESTRAFFLE = 1");

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
        PublicIncludePaths.Add("Planet/GamePlay/UI/Hover");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Hover/DestroyProgress");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Hover/FightingTips");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Menus");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Menus/GroupManagger");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Menus/Backpacks");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Menus/Backpacks/BackpackIcons");
        PublicIncludePaths.Add("Planet/GamePlay/UI/MenuS/CreateMenuS");
        PublicIncludePaths.Add("Planet/GamePlay/UI/MenuS/DragDropOperations");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Menus/SkillsMenu");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Menus/TalentAllocation");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Menus/CoinList");
        PublicIncludePaths.Add("Planet/GamePlay/UI/Menus/RaffleMenu");
        PublicIncludePaths.Add("Planet/GamePlay/UI/MainUI");
        PublicIncludePaths.Add("Planet/GamePlay/UI/MainUI/EffectsList");
        PublicIncludePaths.Add("Planet/GamePlay/UI/MainUI/HUD");
        PublicIncludePaths.Add("Planet/GamePlay/UI/MainUI/HUD/Team");
        PublicIncludePaths.Add("Planet/GamePlay/UI/MainUI/HUD/GetItemInfos");
        PublicIncludePaths.Add("Planet/GamePlay/UI/MainUI/PawnState");
        PublicIncludePaths.Add("Planet/GamePlay/UI/MainUI/PawnState/ActionStateHUD");
        PublicIncludePaths.Add("Planet/GamePlay/UI/MainUI/PawnState/BuildingStateHUD");
        PublicIncludePaths.Add("Planet/GamePlay/UI/UMGs");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Helper_RootMotionSource");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Helper_Tasks");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Helper_Async");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/BasicFutures");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Communication");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Active");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Consumables");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Passive");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Weapon");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Element");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Talent");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/ToolFutures");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Tools");
        PublicIncludePaths.Add("Planet/GamePlay/Animation");
        PublicIncludePaths.Add("Planet/GamePlay/Controller");
        PublicIncludePaths.Add("Planet/GamePlay/GameInstance");
        PublicIncludePaths.Add("Planet/GamePlay/GameplayTags");
        PublicIncludePaths.Add("Planet/GamePlay/PlayerState");
        PublicIncludePaths.Add("Planet/GamePlay/Animation/Horse");
        PublicIncludePaths.Add("Planet/GamePlay/Animation/Human");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Biomes");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Consumables");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Equipments");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Equipments/Tools");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Equipments/Weapons");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Building");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Building/BuildingArea");
        PublicIncludePaths.Add("Planet/GamePlay/SceneTools/Projectile");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/ToolsInteractiveComponent");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Horse");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Human");
        PublicIncludePaths.Add("Planet/GamePlay/WorldSetting");
        PublicIncludePaths.Add("Planet/GamePlay/PlayerCameraManager");
        PublicIncludePaths.Add("Planet/GamePlay/RaffleSystem");
        PublicIncludePaths.Add("Planet/Command");
        PublicIncludePaths.Add("Planet/Common");
        PublicIncludePaths.Add("Planet/SceneUnit");
        PublicIncludePaths.Add("Planet/AssetRefMap");
        PublicIncludePaths.Add("Planet/InputProcessor/InputComponent");
        PublicIncludePaths.Add("Planet/InputProcessor/BirdProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/HorseProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/HumanProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/HumanProcessor/ActionProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/HumanProcessor/InteractionProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/HumanProcessor/BuildingProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/HumanProcessor/BuildingProcessor/PlacingBuindingsProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/HumanProcessor/BuildingProcessor/UseToolsProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/HumanProcessor/ViewMenusProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/VehicleProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/VehicleProcessor/4WheeledVehicle");

        PrivateIncludePaths.Add("Planet/Private");

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
            "GameplayTags",
            "GameplayTasks",
            "GameplayAbilities",
            "GameplayStateTreeModule",
            "AIModule",
            "StateTreeModule",
            "NavigationSystem",
            "ChaosVehicles",
            "ActorSequence",

            "VoxelCore",
            "VoxelGraphCore",
            "MMT",
            "FlyingNavSystem" ,

            "Tools",
        });

        PublicDependencyModuleNames.AddRange(new string[] {
            "Gravity",
        });
    }
}
