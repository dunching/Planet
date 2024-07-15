// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include "UIInterfaces.h"
#include "Common/GenerateType.h"

#include "DragDropOperationWidget.generated.h"

class UUserWidget;
class UToolUnit;
class USkillUnit;
class UConsumableUnit;

struct FStreamableHandle;

/**
 *
 */
UCLASS()
class PLANET_API UDragDropOperationWidget :
	public UMyUserWidget,
	public IToolsIconInterface
{
	GENERATED_BODY()

public:

	UDragDropOperationWidget(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit * BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void SetNum(int32 NewNum);

	void SetItemType(UBasicUnit* ToolSPtr);
	
	void ResetSize(const FVector2D& Size);

protected:

private:

	UWeaponUnit* WeaponUnitSPtr;
	
	UToolUnit* ToolUnitSPtr;
	
	USkillUnit* SkillUnitSPtr;

	UConsumableUnit* ConsumableUnitSPtr;

};
