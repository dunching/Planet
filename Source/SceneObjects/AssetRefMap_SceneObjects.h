// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "SceneElement.h"

#include "AssetRefMap_SceneObjects.generated.h"

class UToolUnit;
class UWeaponUnit;
class USkillUnit;

UCLASS(BlueprintType, Blueprintable)
class SCENEOBJECTS_API UAssetRefMap_SceneObjects : public UObject
{
	GENERATED_BODY()
public:

	static UAssetRefMap_SceneObjects* GetInstance();
	
#pragma region SceneTools
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SceneTools")
	TMap<EToolUnitType, TSubclassOf<UToolUnit>>EquipmentToolsMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SceneTools")
	TMap<EWeaponUnitType, TSubclassOf<UWeaponUnit>>WeaponToolsMap;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "SceneTools")
	TMap<ESkillUnitType, TSubclassOf<USkillUnit>>SkillToolsMap;
#pragma endregion 

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ValidAreaCheckBoxComponnetTag;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PostProcessVolume_Skill_Tag;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<EMaterialInstanceType, TSoftObjectPtr<UMaterialInstance>>MaterialInsSoftPath;

};

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UAssetRefMap_SceneObjects_Interface : public UInterface
{
	GENERATED_BODY()
};


class SCENEOBJECTS_API IAssetRefMap_SceneObjects_Interface
{
	GENERATED_BODY()

public:

	virtual UAssetRefMap_SceneObjects* GetInstance() = 0;

protected:

private:

};
