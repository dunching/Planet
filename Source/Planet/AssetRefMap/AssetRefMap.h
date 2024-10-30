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
class URaffleMenu;
class UPawnStateActionHUD;
class UPawnStateBuildingHUD;
class UItemsDragDropOperation;
class UDragDropOperationWidget;
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

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UAssetRefMap : public UObject
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

#pragma region MenusUI
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UDragDropOperationWidget>DragDropOperationWidgetClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UItemsDragDropOperation>ItemsDragDropOperationClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UMenuLayout>MenuLayoutClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UCharacterTitle>AIHumanInfoClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UFocusIcon>FocusIconClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UCharacterRisingTips>FightingTipsClass;
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
