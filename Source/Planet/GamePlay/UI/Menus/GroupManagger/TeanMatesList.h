// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "SceneElement.h"

#include "TeanMatesList.generated.h"

struct FStreamableHandle;

class UBasicUnit;
class UGourpMateUnit;

/**
 *
 */
UCLASS()
class PLANET_API UTeanMatesList : public UUserWidget, public IItemsMenuInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

protected:

	virtual void ResetUIByData()override;

};
