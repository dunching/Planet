// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "Common/GenerateType.h"
#include <SceneElement.h>

#include "GroupManaggerMenu.generated.h"

struct FSceneObjContainer;

/**
 *
 */
UCLASS()
class PLANET_API UGroupManaggerMenu : public UMyUserWidget, public IItemsMenuInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	AHumanCharacter* HumanCharacterPtr = nullptr;

protected:

	virtual void ResetUIByData()override;

private:

	void ResetGroupmates();

};
