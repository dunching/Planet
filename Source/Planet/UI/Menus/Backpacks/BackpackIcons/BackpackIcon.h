// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"
#include "MyUserWidget.h"
#include "Common/GenerateType.h"
#include "SceneElement.h"

#include "BackpackIcon.generated.h"

struct FStreamableHandle;

struct FBasicProxy;
struct FToolProxy;
struct FCharacterProxy;

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

	using FOnDragDelegate = TCallbackHandleContainer<void(bool, const TSharedPtr<FBasicProxy>&)>;

	using FOnAllocationCharacterUnitChangedHandle = TCallbackHandleContainer<void(const TSharedPtr<FCharacterProxy>&)>::FCallbackHandleSPtr;

	UBackpackIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;
	
	FOnDragDelegate OnDragDelegate;

protected:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation
	)override;
	
	UFUNCTION()
	virtual void OnDroped(UDragDropOperation* Operation);

	virtual	void SetItemType(FBasicProxy* BasicUnitPtr);

	virtual void OnAllocationCharacterUnitChanged(const TSharedPtr<FCharacterProxy>& AllocationCharacterUnitPtr);

	TSharedPtr<FBasicProxy> BasicUnitPtr = nullptr;

	FOnAllocationCharacterUnitChangedHandle OnAllocationCharacterUnitChangedHandle;

};
