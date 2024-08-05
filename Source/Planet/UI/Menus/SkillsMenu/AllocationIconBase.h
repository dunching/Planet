// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Blueprint/IUserObjectListEntry.h>

#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "Common/GenerateType.h"

#include "SceneElement.h"

#include "AllocationIconBase.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

class UBasicUnit;
class UCharacterUnit;

UCLASS()
class PLANET_API UAllocationIconBase :
	public UMyUserWidget,
	public IUnitIconInterface,
	public IUserObjectListEntry
{
	GENERATED_BODY()

public:

	// ¾ÉµÄUnit£¬ÐÂµÄUnit
	using FOnResetUnit = TCallbackHandleContainer<void(UBasicUnit*, UBasicUnit*)>;

	UAllocationIconBase(const FObjectInitializer& ObjectInitializer);

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject)override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	virtual void OnDragIcon(bool bIsDragging, UBasicUnit* UnitPtr);

	virtual void SublingIconUnitChanged(UBasicUnit* UnitPtr);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Socket")
	FGameplayTag IconSocket;

	FOnResetUnit OnResetUnit;

	bool bPaseInvokeOnResetUnitEvent = false;

	UBasicUnit* BasicUnitPtr = nullptr;

protected:

	virtual void SetItemType();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Adaptation UnitType")
	FGameplayTag UnitType = FGameplayTag::EmptyTag;

};