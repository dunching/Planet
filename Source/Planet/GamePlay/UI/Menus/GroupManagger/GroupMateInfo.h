// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "SceneElement.h"

#include "GroupMateInfo.generated.h"

struct FStreamableHandle;

class UBasicUnit;
class UGourpMateUnit;

/**
 *
 */
UCLASS()
class PLANET_API UGroupMateInfo :
	public UUserWidget,
	public IToolsIconInterface
{
	GENERATED_BODY()

public:

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

private:

	UGourpMateUnit* GroupMateUnitPtr = nullptr;

	TSharedPtr<FStreamableHandle> AsyncLoadTextureHandle;

};
