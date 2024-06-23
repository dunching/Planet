// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"
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
	public UUserWidget,
	public IToolsIconInterface
{
	GENERATED_BODY()

public:

	UBackpackIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

protected:

	void SetNum(int32 NewNum);

	void SetItemType();

	void SetValue(int32 Value);

	void ResetSize(const FVector2D& Size);

	virtual void NativeConstruct()override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)override;

private:

	UToolUnit* ToolUnitPtr = nullptr;

	TSharedPtr<FStreamableHandle> AsyncLoadTextureHandle;

};
