// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"
#include "MyUserWidget.h"
#include "Common/GenerateType.h"
#include "SceneElement.h"

#include "BackpackIcon.generated.h"

struct FStreamableHandle;

class UBasicUnit;
class UToolUnit;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackIcon :
	public UMyUserWidget,
	public IToolsIconInterface
{
	GENERATED_BODY()

public:

	UBackpackIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

};
