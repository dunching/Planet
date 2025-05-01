#include "AllocationIconBase.h"

#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/Overlay.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"

#include "AssetRefMap.h"
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "Common/ProxyIcon.h"

struct FAllocationIconBase : public TStructVariable<FAllocationIconBase>
{
	const FName Content = TEXT("Content");

	const FName Enable = TEXT("Enable");

	const FName ProxyIcon = TEXT("ProxyIcon");
};

UAllocationIconBase::UAllocationIconBase(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
}

void UAllocationIconBase::NativeOnListItemObjectSet(
	UObject* ListItemObject
)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	InvokeReset(Cast<ThisClass>(ListItemObject));
}

void UAllocationIconBase::InvokeReset(
	UUserWidget* InBasicProxyPtr
)
{
	if (InBasicProxyPtr)
	{
		auto NewPtr = Cast<ThisClass>(InBasicProxyPtr);
		if (NewPtr)
		{
			OnResetProxy = NewPtr->OnResetProxy;
			ProxyType = NewPtr->ProxyType;
		}
	}
}

void UAllocationIconBase::ResetToolUIByData(
	const TSharedPtr<FBasicProxy>& InBasicProxyPtr
)
{
	if (InBasicProxyPtr == DynamicCastSharedPtr<FBasicProxy>(BasicProxyPtr))
	{
		return;
	}

	auto PreviousProxyPtr = BasicProxyPtr;
	TSharedPtr<IProxy_Allocationble> NewProxyPtr = nullptr;
	if (InBasicProxyPtr && InBasicProxyPtr->GetProxyType().MatchesTag(ProxyType))
	{
		NewProxyPtr = DynamicCastSharedPtr<IProxy_Allocationble>(InBasicProxyPtr);
	}

	if (!bPaseInvokeOnResetProxyEvent)
	{
		OnResetProxy.ExcuteCallback(PreviousProxyPtr, NewProxyPtr, IconSocket);
	}

	BasicProxyPtr = NewProxyPtr;

	if (!bPaseInvokeOnResetProxyEvent)
	{
		OnResetData(this);
	}

	SetItemType();
}

void UAllocationIconBase::EnableIcon(
	bool bIsEnable
)
{
}

void UAllocationIconBase::OnDragIcon(
	bool bIsDragging,
	const TSharedPtr<IProxy_Allocationble>& ProxyPtr
)
{
	if (bIsDragging)
	{
		if (ProxyPtr && DynamicCastSharedPtr<FBasicProxy>( ProxyPtr)->GetProxyType().MatchesTag(ProxyType))
		{
			EnableIcon(true);
		}
		else
		{
			EnableIcon(false);
		}
	}
	else
	{
		EnableIcon(true);
	}
}

void UAllocationIconBase::SublingIconProxyChanged(
	const TSharedPtr<IProxy_Allocationble>& ProxyPtr
)
{
	if (BasicProxyPtr && (BasicProxyPtr == ProxyPtr))
	{
		ResetToolUIByData(nullptr);
	}
}

void UAllocationIconBase::SetItemType()
{
	auto ProxyIconPtr = Cast<UProxyIcon>(GetWidgetFromName(FAllocationIconBase::Get().ProxyIcon));
	if (ProxyIconPtr)
	{
		ProxyIconPtr->ResetToolUIByData(DynamicCastSharedPtr<FBasicProxy>(BasicProxyPtr));
	}
}

FReply UAllocationIconBase::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		ResetToolUIByData(nullptr);
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

bool UAllocationIconBase::NativeOnDrop(
	const FGeometry& InGeometry,
	const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation
)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (InOperation->IsA(UAllocationableProxyDragDropOperation::StaticClass()))
	{
		auto WidgetDragPtr = Cast<UAllocationableProxyDragDropOperation>(InOperation);
		if (WidgetDragPtr)
		{
			ResetToolUIByData(DynamicCastSharedPtr<FBasicProxy>(WidgetDragPtr->SceneToolSPtr));

			WidgetDragPtr->OnDrop.Broadcast(WidgetDragPtr);
		}
	}

	return true;
}

void UAllocationIconBase::NativeOnDragDetected(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation
)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	auto BaseItemClassPtr = UAssetRefMap::GetInstance()->DragDropOperationWidgetClass;

	if (BaseItemClassPtr)
	{
		// auto DragWidgetPtr = CreateWidget<UAllocationableProxyDragDropOperationWidget>(this, BaseItemClassPtr);
		// if (DragWidgetPtr)
		// {
		// 	DragWidgetPtr->ResetSize(InGeometry.Size);
		// 	DragWidgetPtr->ResetToolUIByData(BasicProxyPtr);
		//
		// 	auto WidgetDragPtr = Cast<UAllocationableProxyDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UAllocationableProxyDragDropOperation::StaticClass()));
		// 	if (WidgetDragPtr)
		// 	{
		// 	}
		// }
	}
}
