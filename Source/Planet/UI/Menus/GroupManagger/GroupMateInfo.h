// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include "UIInterfaces.h"
#include "Common/GenerateType.h"
#include "SceneElement.h"

#include "GroupMateInfo.generated.h"

struct FStreamableHandle;

class UBasicUnit;
class UCharacterUnit;

/**
 *
 */
UCLASS()
class PLANET_API UGroupMateInfo :
	public UMyUserWidget,
	public IUnitIconInterface
{
	GENERATED_BODY()

public:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

private:

	UCharacterUnit* GroupMateUnitPtr = nullptr;

};