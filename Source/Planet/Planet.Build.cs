// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Planet : ModuleRules
{
	public Planet(ReadOnlyTargetRules Target) : base(Target)
    {
        //PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        //PCHUsage = PCHUsageMode.UseSharedPCHs;
        PCHUsage = PCHUsageMode.Default;

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
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Helper_Tasks");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Helper_Async");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/BasicFutures");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Communication");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Active");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Passive");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Weapon");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Element");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Talent");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/ToolFutures");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Consumables");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/CharacterState");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/CharacterState/DurationProperty");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/CharacterState/DurationState");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/CharacterState/RootMotion");
        PublicIncludePaths.Add("Planet/GamePlay/Controller");
        PublicIncludePaths.Add("Planet/GamePlay/Component");
        PublicIncludePaths.Add("Planet/GamePlay/GameInstance");
        PublicIncludePaths.Add("Planet/GamePlay/GameMode");
        PublicIncludePaths.Add("Planet/GamePlay/GameState");
        PublicIncludePaths.Add("Planet/GamePlay/GameplayTags");
        PublicIncludePaths.Add("Planet/GamePlay/PlayerState");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/ToolsInteractiveComponent");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Horse");
        PublicIncludePaths.Add("Planet/GamePlay/Pawn/Human");
        PublicIncludePaths.Add("Planet/GamePlay/WorldSetting");
        PublicIncludePaths.Add("Planet/GamePlay/PlayerCameraManager");
        PublicIncludePaths.Add("Planet/RootMotionSource");
        PublicIncludePaths.Add("Planet/Generator");
        PublicIncludePaths.Add("Planet/Generator/AI_Colony");
        PublicIncludePaths.Add("Planet/Animation");
        PublicIncludePaths.Add("Planet/Animation/Horse");
        PublicIncludePaths.Add("Planet/Animation/Human");
        PublicIncludePaths.Add("Planet/UI");
        PublicIncludePaths.Add("Planet/UI/Hover");
        PublicIncludePaths.Add("Planet/UI/Hover/DestroyProgress");
        PublicIncludePaths.Add("Planet/UI/Hover/FightingTips");
        PublicIncludePaths.Add("Planet/UI/Menus");
        PublicIncludePaths.Add("Planet/UI/Menus/GroupManagger");
        PublicIncludePaths.Add("Planet/UI/Menus/Backpacks");
        PublicIncludePaths.Add("Planet/UI/Menus/Backpacks/BackpackIcons");
        PublicIncludePaths.Add("Planet/UI/MenuS/CreateMenuS");
        PublicIncludePaths.Add("Planet/UI/MenuS/DragDropOperations");
        PublicIncludePaths.Add("Planet/UI/Menus/SkillsMenu");
        PublicIncludePaths.Add("Planet/UI/Menus/TalentAllocation");
        PublicIncludePaths.Add("Planet/UI/Menus/CoinList");
        PublicIncludePaths.Add("Planet/UI/Menus/RaffleMenu");
        PublicIncludePaths.Add("Planet/UI/HUD");
        PublicIncludePaths.Add("Planet/UI/HUD/EffectsList");
        PublicIncludePaths.Add("Planet/UI/HUD/Team");
        PublicIncludePaths.Add("Planet/UI/HUD/GetItemInfos");
        PublicIncludePaths.Add("Planet/UI/HUD/PawnState");
        PublicIncludePaths.Add("Planet/UI/HUD/PawnState/ActionStateHUD");
        PublicIncludePaths.Add("Planet/UI/HUD/PawnState/BuildingStateHUD");
        PublicIncludePaths.Add("Planet/UI/UMGs");
        PublicIncludePaths.Add("Planet/AI");
        PublicIncludePaths.Add("Planet/AI/AITask");
        PublicIncludePaths.Add("Planet/AI/EQS");
        PublicIncludePaths.Add("Planet/AI/STE");
        PublicIncludePaths.Add("Planet/AI/STT");
        PublicIncludePaths.Add("Planet/Navgation");
        PublicIncludePaths.Add("Planet/GroupsManagger");
        PublicIncludePaths.Add("Planet/SPlineActor");
        PublicIncludePaths.Add("Planet/SceneTools");
        PublicIncludePaths.Add("Planet/SceneTools/Biomes");
        PublicIncludePaths.Add("Planet/SceneTools/Consumables");
        PublicIncludePaths.Add("Planet/SceneTools/Equipments");
        PublicIncludePaths.Add("Planet/SceneTools/Equipments/Tools");
        PublicIncludePaths.Add("Planet/SceneTools/Equipments/Weapons");
        PublicIncludePaths.Add("Planet/SceneTools/Building");
        PublicIncludePaths.Add("Planet/SceneTools/Building/BuildingArea");
        PublicIncludePaths.Add("Planet/SceneTools/Projectile");
        PublicIncludePaths.Add("Planet/SceneTools/ResourceBox");
        PublicIncludePaths.Add("Planet/RaffleSystem");
        PublicIncludePaths.Add("Planet/Command");
        PublicIncludePaths.Add("Planet/Common");
        PublicIncludePaths.Add("Planet/DelayTask");
        PublicIncludePaths.Add("Planet/NetReplicateData");
        PublicIncludePaths.Add("Planet/AssetRefMap");
        PublicIncludePaths.Add("Planet/GameOptions");
        PublicIncludePaths.Add("Planet/CameraTrailHelper");
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
            "NetCore",

            "Tools",
        });

        PublicDependencyModuleNames.AddRange(new string[] {
            "Gravity",
            "Weather",
            "Utils",

        });
    }
}
