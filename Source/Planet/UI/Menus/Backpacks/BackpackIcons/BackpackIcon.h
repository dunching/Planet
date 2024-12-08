// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/IUserObjectListEntry.h"

#include "UIInterfaces.h"
#include "MyUserWidget.h"
#include "Common/GenerateType.h"
#include "ItemProxy_Minimal.h"

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
	public IItemProxyIconInterface,
	public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	using FOnDragDelegate = TCallbackHandleContainer<void(bool, const TSharedPtr<FBasicProxy>&)>;

	using FOnAllocationCharacterProxyChangedHandle = TCallbackHandleContainer<void(const TWeakPtr<FCharacterProxy>&)>::FCallbackHandleSPtr;

	UBackpackIcon(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject)override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

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

	virtual	void SetItemType(FBasicProxy* BasicProxyPtr);

	virtual void OnAllocationCharacterProxyChanged(const TWeakPtr<FCharacterProxy>& AllocationCharacterProxyPtr);

	TSharedPtr<FBasicProxy> BasicProxyPtr = nullptr;

	FOnAllocationCharacterProxyChangedHandle OnAllocationCharacterProxyChangedHandle;

};
