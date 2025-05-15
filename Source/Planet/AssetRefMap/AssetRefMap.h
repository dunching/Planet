// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "ItemProxy_Minimal.h"

#include "AssetRefMap.generated.h"

class UTexture2D;
class USoundMix;
class USoundClass;
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
class UFocusIcon;
class UMarkPoints;
class UCharacterRisingTips;
class UInteractionList;
class AHumanCharacter;
class AHorseCharacter;
class AGeneratorNPC;
class UMainMenuLayout;
class UGE_Damage;
class UGE_Duration;
class UGE_Damage_Callback;
class ATargetPoint_Runtime;
class AGuideThread_Branch;
class AGuideThread_Challenge;

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UAssetRefMap : public UPrimaryDataAsset
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
#pragma endregion MenusUI

#pragma region GEs
	/**
	 * 一次性
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect>OnceGEClass;

	/**
	 * 周期性
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect>DurationGEClass;

	/**
	 * 永久
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect>ForeverGEClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect>DamageCallbackClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect>SuppressClass;
#pragma endregion 
	
#pragma region Audios
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Audios")
	TSoftObjectPtr<USoundMix>SoundMixRef;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Audios")
	TSoftObjectPtr<USoundClass>BGMSoundClassRef;
#pragma endregion 
	
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
	TSubclassOf<AGuideThread_Branch> GuideBranchThreadTestClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideLine")
	TSubclassOf<AGuideThread_Branch> GuideBranchThreadTest1Class;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideLine")
	TSubclassOf<AGuideThread_Branch> GuideBranchThreadTest2Class;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GuideLine")
	TSubclassOf<AGuideThread_Challenge> GuideThreadChallengeActorClass;
#endif
};

FString GetVoxelWorldSlot();
