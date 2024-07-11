// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "SceneElement.h"

#include "BackpackIconWrapper.generated.h"

struct FStreamableHandle;

class UBasicUnit;
class UToolUnit;
class UBackpackConsumableIcon;
class UBackpackToolIcon;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackIconWrapper :
	public UUserWidget,
	public IToolsIconInterface
{
	GENERATED_BODY()

public:

	UBackpackIconWrapper(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	UBasicUnit* TargetBasicUnitPtr = nullptr;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackToolIcon> ToolUnitClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Icon Class")
	TSubclassOf<UBackpackConsumableIcon> ConsumableUnitClass;

};
