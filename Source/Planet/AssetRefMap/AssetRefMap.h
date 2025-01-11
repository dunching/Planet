// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "ItemProxy_Minimal.h"

#include "AssetRefMap.generated.h"

class UTexture2D;
class UStaticMesh;
class UMaterialInstance;
class USkeletalMesh;
class UGameplayEffect;

class UMainUILayout;
class AGroupSharedInfo;
class URaffleMenu;
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
class UFocusIcon;
class UCharacterRisingTips;
class AHumanCharacter;
class AHorseCharacter;
class AGeneratorNPC;
class UMenuLayout;
class UGE_Damage;
class ATargetPoint_Runtime;

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
	TSubclassOf<AGroupSharedInfo>GroupSharedInfoClass;

#pragma region MenusUI
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UItemProxyDragDropOperationWidget>DragDropOperationWidgetClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UAllocationableProxyDragDropOperationWidget>AllocationableProxyDragDropOperationWidgetClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UItemProxyDragDropOperation>ItemsDragDropOperationClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UMenuLayout>MenuLayoutClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UCharacterTitle>AIHumanInfoClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UFocusIcon>FocusIconClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UCharacterRisingTips>FightingTipsClass;
#pragma endregion MenusUI

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGameplayEffect>DurationGEClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<UGE_Damage>DamageClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GE")
	TSubclassOf<ATargetPoint_Runtime>TargetPoint_RuntimeClass;

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

};

FString GetVoxelWorldSlot();
