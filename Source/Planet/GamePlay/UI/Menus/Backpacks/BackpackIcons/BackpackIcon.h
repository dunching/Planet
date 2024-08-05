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
class UCharacterUnit;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackIcon :
	public UMyUserWidget,
	public IUnitIconInterface
{
	GENERATED_BODY()

public:

	using FOnDragDelegate = TCallbackHandleContainer<void(bool, UBasicUnit*)>;

	UBackpackIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;
	
	virtual void SetAllocationCharacterUnit(UCharacterUnit* AllocationCharacterUnitPtr);

	FOnDragDelegate OnDragDelegate;

protected:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation
	)override;
	
	UFUNCTION()
	virtual void OnDroped(UDragDropOperation* Operation);

	virtual	void SetItemType(UBasicUnit* BasicUnitPtr);

	UBasicUnit *BasicUnitPtr = nullptr;

};
