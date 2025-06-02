// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Planet : ModuleRules
{
	public Planet(ReadOnlyTargetRules Target) : base(Target)
	{
		// Include What You Use (IWYU)
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		DefaultBuildSettings = BuildSettingsVersion.Latest;

		UndefinedIdentifierWarningLevel = WarningLevel.Error;
		bWarningsAsErrors = true;
		bEnableExceptions = true;

		CppStandard = CppStandardVersion.Cpp20;

		bUseRTTI = true;

		if (Target.Platform == UnrealTargetPlatform.Win64)
		{
		}

		PublicDefinitions.Add("TESTPLAYERCHARACTERHOLDDATA = 0");
		PublicDefinitions.Add("TESTAICHARACTERHOLDDATA = 1");
		PublicDefinitions.Add("TESTRESOURCEBOXHOLDDATA = 1");
		PublicDefinitions.Add("TESTRAFFLE = 1");

		PublicIncludePaths.Add("Planet");
		PublicIncludePaths.Add("Planet/OpenWorld");
		PublicIncludePaths.Add("Planet/GamePlay");
		PublicIncludePaths.Add("Planet/GamePlay/GameplayCamera");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/GameplayEffectDataModify");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/GameplayAbilityTargetTypes");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/AttributeSet");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/GameplayEffects");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Helper_Tasks");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Helper_Async");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/BasicFutures");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Active");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Passive");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Weapon");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Element");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Skills/Talent");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/ToolFutures");
		PublicIncludePaths.Add("Planet/GamePlay/AbilitySystem/Consumables");
		PublicIncludePaths.Add("Planet/GamePlay/Controller");
		PublicIncludePaths.Add("Planet/GamePlay/Controller/Player");
		PublicIncludePaths.Add("Planet/GamePlay/Controller/NPC");
		PublicIncludePaths.Add("Planet/GamePlay/Component");
		PublicIncludePaths.Add("Planet/GamePlay/GameInstance");
		PublicIncludePaths.Add("Planet/GamePlay/GameMode");
		PublicIncludePaths.Add("Planet/GamePlay/GameState");
		PublicIncludePaths.Add("Planet/GamePlay/PlayerState");
		PublicIncludePaths.Add("Planet/GamePlay/Pawn");
		PublicIncludePaths.Add("Planet/GamePlay/Pawn/Horse");
		PublicIncludePaths.Add("Planet/GamePlay/Pawn/Human");
		PublicIncludePaths.Add("Planet/GamePlay/WorldSetting");
		PublicIncludePaths.Add("Planet/GamePlay/PlayerCameraManager");
		PublicIncludePaths.Add("Planet/GamePlay/GameViewportClient");
		PublicIncludePaths.Add("Planet/RootMotionSource");
		PublicIncludePaths.Add("Planet/Animation");
		PublicIncludePaths.Add("Planet/Animation/Horse");
		PublicIncludePaths.Add("Planet/Animation/Human");
		PublicIncludePaths.Add("Planet/Transition");
		PublicIncludePaths.Add("Planet/UI");
		PublicIncludePaths.Add("Planet/UI/Common");
		PublicIncludePaths.Add("Planet/UI/Hover");
		PublicIncludePaths.Add("Planet/UI/Hover/DestroyProgress");
		PublicIncludePaths.Add("Planet/UI/Hover/CharacterRisingTips");
		PublicIncludePaths.Add("Planet/UI/HUD");
		PublicIncludePaths.Add("Planet/UI/HUD/EffectsList");
		PublicIncludePaths.Add("Planet/UI/HUD/GetItemInfos");
		PublicIncludePaths.Add("Planet/UI/HUD/FocusTitle");
		PublicIncludePaths.Add("Planet/UI/UMGs");
		PublicIncludePaths.Add("Planet/UI/Layout");
		PublicIncludePaths.Add("Planet/UI/Layout/InteractionList");
		PublicIncludePaths.Add("Planet/UI/Layout/Team");
		PublicIncludePaths.Add("Planet/UI/Layout/GuideList");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/ActionLayout");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/BuildingLayout");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/EndangeredLayout");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/InteractionOptionsLayout");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/InteractionConversationLayout");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/Common");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/Common/CoinList");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/Common/DragDropOperations");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/GroupManagger");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/CreateMenuS");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/AllocationMenu");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/AllocationMenu/Backpacks");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/AllocationMenu/Backpacks/BackpackIcons");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/TalentAllocation");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/RaffleMenu");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/ViewMap");
		PublicIncludePaths.Add("Planet/UI/Layout/Layout/MenusLayout/ViewTasks");
		PublicIncludePaths.Add("Planet/AI");
		PublicIncludePaths.Add("Planet/AI/AITask");
		PublicIncludePaths.Add("Planet/AI/EQS");
		PublicIncludePaths.Add("Planet/AI/STE");
		PublicIncludePaths.Add("Planet/AI/STT");
		PublicIncludePaths.Add("Planet/AI/StateTreeComponent");
		PublicIncludePaths.Add("Planet/Navgation");
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
		PublicIncludePaths.Add("Planet/NetReplicateData");
		PublicIncludePaths.Add("Planet/NetReplicateData/ItemProxys");
		PublicIncludePaths.Add("Planet/AssetRefMap");
		PublicIncludePaths.Add("Planet/CameraTrailHelper");

		PrivateIncludePaths.Add("Planet/Private");

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
			"InputCore",
			"SlateCore",
			"Slate",
			"UMG",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"NavigationSystem",
			"ChaosVehicles",
			"ActorSequence",
			"NetCore",
			"ShaderConductor",

			// 引擎插件
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
			"GameplayStateTreeModule",
			"GameplayAbilities",
			"GameplayCameras",
			"Niagara",

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
			"AssetRef",
			"ChallengeSystem",
			"CommonType",
			"GameOptions",
			"GameplayTagsLibrary",
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