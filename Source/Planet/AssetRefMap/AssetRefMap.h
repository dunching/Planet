// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "AssetRefBase.h"
#include "GenerateTypes.h"
#include "ItemProxy_Minimal.h"

#include "AssetRefMap.generated.h"

class USoundMix;
class USoundClass;
class UTexture2D;
class UStaticMesh;
class UMaterialInstance;
class USkeletalMesh;
class UGameplayEffect;

class UMainUILayout;
class AGroupManagger;
class URaffleMenu;
class UMainHUDLayout;
class UPawnStateActionHUD;
class UPawnStateBuildingHUD;
class UItemProxyDragDropOperation;
class UItemProxyDragDropOperationWidget;
class UAllocationableProxyDragDropOperationWidget;
class UAllocationToolsMenu;
class UAllocationSkillsMenu;
class UBackpackMenu;
class UCharacterTitle;
class UEffectsList;
class UProgressTips;
class UTalentAllocation;
class UGroupManaggerMenu;
class UHUD_TeamInfo;
class URegionPromt;
class UUpgradePromt;
class UFocusIcon;
class UMarkPoints;
class UUpgradeBoder;
class UCharacterRisingTips;
class UInteractionList;
class AHumanCharacter;
class AHorseCharacter;
class AGeneratorNPC;
class UMainMenuLayout;
class UGE_Damage;
class UGE_Duration;
class UGE_Damage_Callback;
class AMinimapSceneCapture2D;
class ATargetPoint_Runtime;
class AQuestChain_Branch;
class AGuideThread_Challenge;

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UAssetRefMap : public UAssetRefBase
{
	GENERATED_BODY()
public:

	UAssetRefMap();

	static UAssetRefMap* GetInstance();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PawnClass")
	TSubclassOf<AHumanCharacter>HumanClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PawnClass")
	TSubclassOf<AHumanCharacter>TestNPC_HumanClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "PawnClass")
	TSubclassOf<AHorseCharacter>HorseClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GroupSharedInfoClass")
	TSubclassOf<AGroupManagger>GroupSharedInfoClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Minimap")
	TSubclassOf<AMinimapSceneCapture2D>MinimapSceneCapture2DClass;

#pragma region MenusUI
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UItemProxyDragDropOperationWidget>DragDropOperationWidgetClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UAllocationableProxyDragDropOperationWidget>AllocationableProxyDragDropOperationWidgetClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UItemProxyDragDropOperation>ItemsDragDropOperationClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMainMenuLayout>MenuLayoutClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCharacterTitle>AIHumanInfoClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UFocusIcon>FocusIconClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInteractionList>InteractionList;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<URegionPromt>RegionPromtClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUpgradeBoder>UpgradeBoderClass;
#pragma endregion MenusUI

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<EMaterialParamNameType, FName>MatParamNameMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<EMaterialParamType, FColor>MatVectirParamMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<TSoftObjectPtr<UStaticMesh>>TreeStaticMeshRefAry;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<EMaterialInstanceType, TSoftObjectPtr<UMaterialInstance>>MaterialInsSoftPath;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ValidAreaCheckBoxComponnetTag;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PostProcessVolume_Skill_Tag;

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideLine")
	TSubclassOf<AQuestChain_Branch> GuideBranchThreadTestClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideLine")
	TSubclassOf<AQuestChain_Branch> GuideBranchThreadTest1Class;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideLine")
	TSubclassOf<AQuestChain_Branch> GuideBranchThreadTest2Class;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideLine")
	TSubclassOf<AGuideThread_Challenge> GuideThreadChallengeActorClass;
#endif
};

FString GetVoxelWorldSlot();
