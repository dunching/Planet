
#include "AllocationIconBase.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include <Kismet/GameplayStatics.h>
#include "Components/CanvasPanel.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/ProgressBar.h"
#include "Kismet/KismetStringLibrary.h"
#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"

#include "UICommon.h"
#include "ItemsDragDropOperation.h"
#include "AssetRefMap.h"
#include "DragDropOperationWidget.h"

struct FAllocationIconBase : public TStructVariable<FAllocationIconBase>
{
	const FName Content = TEXT("Content");

	const FName Enable = TEXT("Enable");

	const FName Icon = TEXT("Icon");
};

UAllocationIconBase::UAllocationIconBase(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UAllocationIconBase::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	InvokeReset(Cast<ThisClass>(ListItemObject));
}

void UAllocationIconBase::InvokeReset(UUserWidget* InBasicUnitPtr)
{
	if (InBasicUnitPtr)
	{
		auto NewPtr = Cast<ThisClass>(InBasicUnitPtr);
		if (NewPtr)
		{
			OnResetUnit = NewPtr->OnResetUnit;
			UnitType = NewPtr->UnitType;
		}
	}
}

void UAllocationIconBase::ResetToolUIByData(const TSharedPtr<FBasicProxy>& InBasicUnitPtr)
{
	if (InBasicUnitPtr == BasicUnitPtr)
	{
		return;
	}

	auto PreviousUnitPtr = BasicUnitPtr;
	TSharedPtr<FBasicProxy> NewUnitPtr = nullptr;
	if (InBasicUnitPtr && InBasicUnitPtr->GetUnitType().MatchesTag(UnitType))
	{
		NewUnitPtr = InBasicUnitPtr;
	}

	if (!bPaseInvokeOnResetUnitEvent)
	{
		OnResetUnit.ExcuteCallback(PreviousUnitPtr, NewUnitPtr);
	}

	BasicUnitPtr = NewUnitPtr;

	SetItemType();
}

void UAllocationIconBase::EnableIcon(bool bIsEnable)
{

}

void UAllocationIconBase::OnDragIcon(bool bIsDragging, const TSharedPtr<FBasicProxy>& UnitPtr)
{
	if (bIsDragging)
	{
		if (UnitPtr && UnitPtr->GetUnitType().MatchesTag(UnitType))
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

void UAllocationIconBase::SublingIconUnitChanged(const TSharedPtr<FBasicProxy>& UnitPtr)
{
	if (BasicUnitPtr && (BasicUnitPtr == UnitPtr))
	{
		ResetToolUIByData(nullptr);
	}
}

void UAllocationIconBase::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FAllocationIconBase::Get().Icon));
	if (ImagePtr)
	{
		if (BasicUnitPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(BasicUnitPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
				{
					ImagePtr->SetBrushFromTexture(BasicUnitPtr->GetIcon().Get());
				}));
		}
		else
		{
			ImagePtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

FReply UAllocationIconBase::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
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

bool UAllocationIconBase::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (InOperation->IsA(UItemsDragDropOperation::StaticClass()))
	{
		auto WidgetDragPtr = Cast<UItemsDragDropOperation>(InOperation);
		if (WidgetDragPtr)
		{
			ResetToolUIByData(WidgetDragPtr->SceneToolSPtr);

			WidgetDragPtr->OnDrop.Broadcast(WidgetDragPtr);
		}
	}

	return true;
}

void UAllocationIconBase::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	auto BaseItemClassPtr = UAssetRefMap::GetInstance()->DragDropOperationWidgetClass;

	if (BaseItemClassPtr)
	{
		auto DragWidgetPtr = CreateWidget<UDragDropOperationWidget>(this, BaseItemClassPtr);
		if (DragWidgetPtr)
		{
			DragWidgetPtr->ResetSize(InGeometry.Size);
			DragWidgetPtr->ResetToolUIByData(BasicUnitPtr);

			auto WidgetDragPtr = Cast<UItemsDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemsDragDropOperation::StaticClass()));
			if (WidgetDragPtr)
			{
			}
		}
	}
}
