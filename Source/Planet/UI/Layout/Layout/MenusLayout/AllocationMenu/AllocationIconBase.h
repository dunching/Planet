// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <Blueprint/IUserObjectListEntry.h>
#include "GameplayTagContainer.h"

#include "UIInterfaces.h"
#include "MyUserWidget.h"

#include "ItemProxy_Minimal.h"

#include "AllocationIconBase.generated.h"

struct FStreamableHandle;
class UDragDropOperation;

struct FBasicProxy;
struct IProxy_Allocationble;
struct FCharacterProxy;

UCLASS()
class PLANET_API UAllocationIconBase :
	public UMyUserWidget,
	public IUserObjectListEntry,
	public IAllocationableProxyIconInterface
{
	GENERATED_BODY()

public:
	// 旧的Proxy，新的Proxy
	using FOnResetProxy =
	TCallbackHandleContainer<void(
		const TSharedPtr<IProxy_Allocationble>&,
		const TSharedPtr<IProxy_Allocationble>&,
		const FGameplayTag&
			
	)>;

	using FOnResetData =
	TCallbackHandleContainer<void(
		UAllocationIconBase*
	)>;

	UAllocationIconBase(
		const FObjectInitializer& ObjectInitializer
	);

	virtual void NativeOnListItemObjectSet(
		UObject* ListItemObject
	) override;

	virtual void InvokeReset(
		UUserWidget* BaseWidgetPtr
	) override;

	virtual void ResetToolUIByData(
		const TSharedPtr<FBasicProxy>& BasicProxyPtr
	) override;

	virtual void EnableIcon(
		bool bIsEnable
	) override;

	virtual void OnDragIcon(
		bool bIsDragging,
		const TSharedPtr<IProxy_Allocationble>& ProxyPtr
	);

	virtual void SublingIconProxyChanged(
		const TSharedPtr<IProxy_Allocationble>& ProxyPtr
	);

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
	) override;

	virtual bool NativeOnDrop(
		const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation
	) override;

	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent,
		UDragDropOperation*& OutOperation
	) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Socket")
	FGameplayTag IconSocket;

	FOnResetProxy OnResetProxy;

	FOnResetData OnResetData;

	bool bPaseInvokeOnResetProxyEvent = false;

	TSharedPtr<IProxy_Allocationble> BasicProxyPtr = nullptr;

protected:
	virtual void SetItemType();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Adaptation ProxyType")
	FGameplayTag ProxyType = FGameplayTag::EmptyTag;
};
