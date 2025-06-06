// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/IUserObjectListEntry.h"

#include "UserWidget_Override.h"
#include "UIInterfaces.h"
#include "GenerateTypes.h"
#include "ItemProxy_Minimal.h"

#include "GroupMateInfo.generated.h"

struct FStreamableHandle;

struct FBasicProxy;
struct FCharacterProxy;

/**
 *
 */
UCLASS()
class PLANET_API UGroupMateInfo :
	public UUserWidget_Override,
	public IItemProxyIconInterface,
	public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject)override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	TSharedPtr < FCharacterProxy> GroupMateProxyPtr = nullptr;

private:

};
