// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "AssetRefMap.generated.h"

class UTexture2D;
class UStaticMesh;
class UMaterialInstance;
class USkeletalMesh;

class UPromptBox;
class UPromptStr;
class UEquipMenu;
class UEquipIcon;
class UEquipIconDrag;
class UBackpackMenu;
class UBackpackIcon;
class UBackpackIconDrag;
class UCreateMenu;
class UCreateItem;
class UCreateQueue;
class UCreateQueueItem;
class UInteractionToAIMenu;
class UInteractionBtn;
class UDestroyProgress;

class UPiginteraction;
class UBuildInteractionWidgetBase;
class UPlaceWidgetBase;

class ARawMaterialBase;
class ABuildingBase;
class AEquipmentBase;
class ASceneObj;
class ABuildingCaptureData;
class AHumanCharacter;

UCLASS(BlueprintType, Blueprintable)
class SHIYU_API UAssetRefMap : public UObject
{
	GENERATED_BODY()
public:

	UAssetRefMap();

	static UAssetRefMap* GetInstance();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TMap<EMaterialParamNameType, FName>MatParamNameMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TMap<EMaterialParamType, FColor>MatVectirParamMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TMap<ERawMaterialType, TSoftObjectPtr<UTexture2D>>RawMaterialImg;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TMap<EBuildingType, TSoftObjectPtr<UTexture2D>>BuildingTypeImg;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TMap<EEquipmentType, TSoftObjectPtr<UTexture2D>>EquipmentTypeImg;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TArray<TSoftObjectPtr<UStaticMesh>>TreeStaticMeshRefAry;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TMap<ERawMaterialType, TSubclassOf<ARawMaterialBase>>RawMaterialClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TMap<EBuildingType, TSubclassOf<ABuildingBase>>BuildingClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TMap<EEquipmentType, TSubclassOf<AEquipmentBase>>EquipmentTypeClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TMap<EMaterialInstanceType, TSoftObjectPtr<UMaterialInstance>>MaterialInsSoftPath;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UEquipMenu>EquipMenuClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UEquipIcon>EquipIconClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UEquipIconDrag>EquipIconDragClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UBackpackMenu>BackpackMenuClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UBackpackIcon>BackpackIconClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UBackpackIconDrag>BackpackIconDragClass;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UCreateMenu>CreateMenuClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UCreateItem>CreateItemClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UCreateQueue>CreateQueueClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UCreateQueueItem>CreateQueueItemClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UPromptBox>PromptClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UPromptStr>PromptStrClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UInteractionToAIMenu>InteractionToAIMenuClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UInteractionBtn>InteractionBtnClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UPiginteraction>PiginteractionClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UBuildInteractionWidgetBase>BuildInteractionWidgetBaseClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UPlaceWidgetBase>PlaceWidgetBaseClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<UDestroyProgress>DestroyProgressClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		TSubclassOf<AHumanCharacter>HumanCharacterClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FName ValidAreaCheckBoxComponnetTag;

};

FString GetVoxelWorldSlot();