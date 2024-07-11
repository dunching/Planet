// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BackpackIcon.h"

#include "BackpackToolIcon.generated.h"

struct FStreamableHandle;

class UBasicUnit;
class UToolUnit;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackToolIcon :
	public UBackpackIcon
{
	GENERATED_BODY()

public:

	UBackpackToolIcon(const FObjectInitializer& ObjectInitializer);

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

	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation
	)override;

private:

	UToolUnit* UnitPtr = nullptr;

	TSharedPtr<FStreamableHandle> AsyncLoadTextureHandle;

};
