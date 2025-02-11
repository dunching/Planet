// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Planet : ModuleRules
{
	public Planet(ReadOnlyTargetRules Target) : base(Target)
    {
        // 这几个有啥区别？
        // PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
        // PCHUsage = PCHUsageMode.UseSharedPCHs;
        // PCHUsage = PCHUsageMode.Default;

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
        PublicIncludePaths.Add("Planet/GuideSystem");
        PublicIncludePaths.Add("Planet/GuideSystem/GuideActor");
        PublicIncludePaths.Add("Planet/GuideSystem/GuideActor/GuideInteraction");
        PublicIncludePaths.Add("Planet/GuideSystem/GuideActor/GuideThread");
        PublicIncludePaths.Add("Planet/GuideSystem/TaskNode");
        PublicIncludePaths.Add("Planet/GuideSystem/TaskNode/TaskNode_Character");
        PublicIncludePaths.Add("Planet/GuideSystem/TaskNode/TaskNode_GuideSystem");
        PublicIncludePaths.Add("Planet/GuideSystem/TaskNode/TaskNode_Interaction");
        PublicIncludePaths.Add("Planet/GuideSystem/STT");
        PublicIncludePaths.Add("Planet/GuideSystem/STT/GuideInteraction");
        PublicIncludePaths.Add("Planet/GuideSystem/STT/GuideThread");
        PublicIncludePaths.Add("Planet/GuideSystem/STE");
        PublicIncludePaths.Add("Planet/GuideSystem/STE/GuideInteraction");
        PublicIncludePaths.Add("Planet/GuideSystem/STE/GuideThread");
        PublicIncludePaths.Add("Planet/GuideSystem/GuideSystem_GameplayTask");
        PublicIncludePaths.Add("Planet/GuideSystem/GuideSystem_GameplayTask/GuideThread");
        PublicIncludePaths.Add("Planet/GuideSystem/GuideSystem_GameplayTask/GuideInteraction");
        PublicIncludePaths.Add("Planet/GamePlay");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/AttributeSet");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/GameplayEffects");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Helper_Tasks");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Helper_Async");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/BasicFutures");
        PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/EffectsCirculation");
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
        PublicIncludePaths.Add("Planet/GamePlay/GroupManagger");
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
        PublicIncludePaths.Add("Planet/UI/Hover/CharacterRisingTips");
        PublicIncludePaths.Add("Planet/UI/HUD");
        PublicIncludePaths.Add("Planet/UI/HUD/EffectsList");
        PublicIncludePaths.Add("Planet/UI/HUD/GetItemInfos");
        PublicIncludePaths.Add("Planet/UI/HUD/FocusTitle");
        PublicIncludePaths.Add("Planet/UI/HUD/PlayerConversationBorder");
        PublicIncludePaths.Add("Planet/UI/UMGs");
        PublicIncludePaths.Add("Planet/UI/Layout");
        PublicIncludePaths.Add("Planet/UI/Layout/InteractionList");
        PublicIncludePaths.Add("Planet/UI/Layout/Team");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/ActionLayout");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/BuildingLayout");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/EndangeredLayout");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/ConversationLayout");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/GroupManagger");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/Backpacks");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/Backpacks/BackpackIcons");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/CreateMenuS");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/DragDropOperations");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/AllocationMenu");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/TalentAllocation");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/CoinList");
        PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/RaffleMenu");
        PublicIncludePaths.Add("Planet/AI");
        PublicIncludePaths.Add("Planet/AI/AITask");
        PublicIncludePaths.Add("Planet/AI/EQS");
        PublicIncludePaths.Add("Planet/AI/STE");
        PublicIncludePaths.Add("Planet/AI/STT");
        PublicIncludePaths.Add("Planet/AI/StateTreeComponent");
        PublicIncludePaths.Add("Planet/Navgation");
        PublicIncludePaths.Add("Planet/GroupsManagger");
        PublicIncludePaths.Add("Planet/SceneHelperActor");
        PublicIncludePaths.Add("Planet/SceneActor");
        PublicIncludePaths.Add("Planet/SceneActor/Biomes");
        PublicIncludePaths.Add("Planet/SceneActor/Consumables");
        PublicIncludePaths.Add("Planet/SceneActor/Equipments");
        PublicIncludePaths.Add("Planet/SceneActor/Equipments/Tools");
        PublicIncludePaths.Add("Planet/SceneActor/Equipments/Weapons");
        PublicIncludePaths.Add("Planet/SceneActor/Building");
        PublicIncludePaths.Add("Planet/SceneActor/Building/BuildingArea");
        PublicIncludePaths.Add("Planet/SceneActor/Projectile");
        PublicIncludePaths.Add("Planet/SceneActor/ResourceBox");
        PublicIncludePaths.Add("Planet/RaffleSystem");
        PublicIncludePaths.Add("Planet/Command");
        PublicIncludePaths.Add("Planet/Common");
        PublicIncludePaths.Add("Planet/DelayTask");
        PublicIncludePaths.Add("Planet/NetReplicateData");
        PublicIncludePaths.Add("Planet/NetReplicateData/ItemProxys");
        PublicIncludePaths.Add("Planet/AssetRefMap");
        PublicIncludePaths.Add("Planet/GameOptions");
        PublicIncludePaths.Add("Planet/CameraTrailHelper");
        PublicIncludePaths.Add("Planet/InputProcessor/InputComponent");
        PublicIncludePaths.Add("Planet/InputProcessor/BirdProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/HorseProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/HumanProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/HumanProcessor/ActionProcessor");
        PublicIncludePaths.Add("Planet/InputProcessor/HumanProcessor/EndangeredProcessor");
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
