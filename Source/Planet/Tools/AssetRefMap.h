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
#pragma endregion MenusUI

#pragma region Tags
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag JumpAbilityTag =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Locomotion.Jump")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag RunningAbilityTag =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Locomotion.Run")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag DashAbilityTag =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Locomotion.Dash")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag DeathingTag = 
		FGameplayTag::RequestGameplayTag(FName(TEXT("Deathing")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag InFightingTag = 
		FGameplayTag::RequestGameplayTag(FName(TEXT("InFighting")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Gameplay Tag")
	FGameplayTag GameplayTag1 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.Continuous")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Gameplay Tag")
	FGameplayTag GameplayTag2 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.WeaponActive")));
#pragma endregion Tags

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

};

FString GetVoxelWorldSlot();
