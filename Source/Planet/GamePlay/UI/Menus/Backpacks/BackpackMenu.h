// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "Common/GenerateType.h"
#include <SceneElement.h>

#include "BackpackMenu.generated.h"

struct FSceneObjContainer;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackMenu : public UUserWidget, public IItemsMenuInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	void SetHoldItemProperty(const FSceneToolsContainer& NewSPHoldItemPerperty);

protected:

	virtual void ResetUIByData()override;

private:

	FSceneToolsContainer SPHoldItemPerpertyPtr;

};
