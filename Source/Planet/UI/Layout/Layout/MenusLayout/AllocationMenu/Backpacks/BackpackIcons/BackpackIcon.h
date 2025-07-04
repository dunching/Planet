// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/IUserObjectListEntry.h"

#include "UIInterfaces.h"
#include "UserWidget_Override.h"
#include "GenerateTypes.h"
#include "ItemProxy_Minimal.h"

#include "BackpackIcon.generated.h"

class UAllocationSkillsMenu;

struct FStreamableHandle;

struct FBasicProxy;
struct FToolProxy;
struct FCharacterProxy;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackIcon :
	public UUserWidget_Override,
	public IItemProxyIconInterface,
	public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	using FOnAllocationCharacterProxyChangedHandle = TCallbackHandleContainer<void(const TWeakPtr<FCharacterProxy>&)>::FCallbackHandleSPtr;

	UBackpackIcon(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject)override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;
	
	UAllocationSkillsMenu * AllocationSkillsMenuPtr= nullptr;
	
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
