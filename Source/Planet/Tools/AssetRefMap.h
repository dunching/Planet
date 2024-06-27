// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "SceneElement.h"

#include "AssetRefMap.generated.h"

class UTexture2D;
class UStaticMesh;
class UMaterialInstance;
class USkeletalMesh;

class UMainUILayout;
class UPawnStateActionHUD;
class UPawnStateBuildingHUD;
class UItemsDragDropOperation;
class UDragDropOperationWidget;
class UAllocationToolsMenu;
class UAllocationSkillsMenu;
class UBackpackMenu;
class UAIHumanInfo;
class UEffectsList;
class UProgressTips;
class UTalentAllocation;
class UGroupManaggerMenu;
class UHUD_TeamInfo;
class UFocusIcon;
class UFightingTips;

class UToolUnit;
class UWeaponUnit;
class USkillUnit;

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UAssetRefMap : public UObject
{
	GENERATED_BODY()
public:

	UAssetRefMap();

	static UAssetRefMap* GetInstance();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<ACharacterBase>HumanClass;

#pragma region SceneTools
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SceneTools")
	TMap<EToolUnitType, TSubclassOf<UToolUnit>>EquipmentToolsMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SceneTools")
	TMap<EWeaponUnitType, TSubclassOf<UWeaponUnit>>WeaponToolsMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SceneTools")
	TMap<ESkillUnitType, TSubclassOf<USkillUnit>>SkillToolsMap;
#pragma endregion 

#pragma region MenusUI
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UDragDropOperationWidget>DragDropOperationWidgetClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UItemsDragDropOperation>ItemsDragDropOperationClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UMainUILayout>MainUILayoutClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UBackpackMenu>BackpackMenuClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UAllocationSkillsMenu>AllocationSkillsMenuClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UPawnStateActionHUD>PawnStateActionHUDClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UPawnStateBuildingHUD>PawnStateBuildingHUDClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UAIHumanInfo>AIHumanInfoClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UFocusIcon>FocusIconClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UEffectsList>EffectsListClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UProgressTips>ProgressTipsClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UTalentAllocation>TalentAllocationClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UGroupManaggerMenu>GroupManaggerMenuClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UHUD_TeamInfo>HUD_TeamInfoClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UFightingTips>FightingTipsClass;
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

};

FString GetVoxelWorldSlot();
