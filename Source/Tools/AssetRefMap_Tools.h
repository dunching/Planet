// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "AssetRefMap_Tools.generated.h"

UCLASS(BlueprintType, Blueprintable)
class TOOLS_API UAssetRefMap_Tools : public UObject
{
	GENERATED_BODY()
public:

	static UAssetRefMap_Tools* GetInstance();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName ValidAreaCheckBoxComponnetTag;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName PostProcessVolume_Skill_Tag;

};

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UAssetRefMap_Tools_Interface : public UInterface
{
	GENERATED_BODY()
};


class TOOLS_API IAssetRefMap_Tools_Interface
{
	GENERATED_BODY()

public:

	virtual UAssetRefMap_Tools* GetInstance() = 0;

protected:

private:

};
