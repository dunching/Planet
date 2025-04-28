#include "BackpackIcon.h"

#include <Kismet/GameplayStatics.h>
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/ProgressBar.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"


#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "ItemProxy_Minimal.h"
#include "GameplayTagsLibrary.h"
#include "UICommon.h"
#include "CharacterAttibutes.h"
#include "GameplayTagsLibrary.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "ItemProxy_Character.h"

struct FBackpackIcon : public TStructVariable<FBackpackIcon>
{
	const FName Content = TEXT("Content");

	const FName Enable = TEXT("Enable");

	const FName Icon = TEXT("Icon");

	const FName AllocationCharacterProxy = TEXT("AllocationCharacterProxy");
};

UBackpackIcon::UBackpackIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void UBackpackIcon::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	InvokeReset(Cast<ThisClass>(ListItemObject));
}

void UBackpackIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
{
}

void UBackpackIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& InBasicProxyPtr)
{
	BasicProxyPtr = InBasicProxyPtr;

	if (BasicProxyPtr)
	{
		SetItemType(BasicProxyPtr.Get());

		if (auto AllocationbleProxySPtr = DynamicCastSharedPtr<FAllocationbleProxy>(InBasicProxyPtr))
		{
			OnAllocationCharacterProxyChangedHandle = AllocationbleProxySPtr->OnAllocationCharacterProxyChanged.AddCallback(
				std::bind(&ThisClass::OnAllocationCharacterProxyChanged, this, std::placeholders::_1)
			);

			OnAllocationCharacterProxyChanged(AllocationbleProxySPtr->GetAllocationCharacterProxy());
		}
	}
}

void UBackpackIcon::EnableIcon(bool bIsEnable)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FBackpackIcon::Get().Enable));
	if (ImagePtr)
	{
		ImagePtr->SetVisibility(bIsEnable ? ESlateVisibility::Hidden : ESlateVisibility::Visible);
	}
}

FReply UBackpackIcon::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UBackpackIcon::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	OnDragDelegate.ExcuteCallback(false, nullptr);
}

void UBackpackIcon::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                         UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	if (auto AllocationbleProxySPtr = DynamicCastSharedPtr<FAllocationbleProxy>(BasicProxyPtr))
	{
		auto BaseItemClassPtr = UAssetRefMap::GetInstance()->AllocationableProxyDragDropOperationWidgetClass;
		if (!BaseItemClassPtr)
		{
			return;
		}

		auto DragWidgetPtr = CreateWidget<UAllocationableProxyDragDropOperationWidget>(this, BaseItemClassPtr);
		if (DragWidgetPtr)
		{
			DragWidgetPtr->ResetSize(InGeometry.Size);
			DragWidgetPtr->ResetToolUIByData(AllocationbleProxySPtr);

			auto WidgetDragPtr = Cast<UAllocationableProxyDragDropOperation>(
				UWidgetBlueprintLibrary::CreateDragDropOperation(UAllocationableProxyDragDropOperation::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->SceneToolSPtr = AllocationbleProxySPtr;
				WidgetDragPtr->bIsInBackpakc = true;
				WidgetDragPtr->OnDrop.AddDynamic(this, &ThisClass::OnDroped);

				OutOperation = WidgetDragPtr;

				OnDragDelegate.ExcuteCallback(true, BasicProxyPtr);
			}
		}
	}
	else
	{
		auto BaseItemClassPtr = UAssetRefMap::GetInstance()->DragDropOperationWidgetClass;
		if (!BaseItemClassPtr)
		{
			return;
		}

		auto DragWidgetPtr = CreateWidget<UItemProxyDragDropOperationWidget>(this, BaseItemClassPtr);
		if (DragWidgetPtr)
		{
			DragWidgetPtr->ResetSize(InGeometry.Size);
			DragWidgetPtr->ResetToolUIByData(BasicProxyPtr);

			auto WidgetDragPtr = Cast<UItemProxyDragDropOperation>(
				UWidgetBlueprintLibrary::CreateDragDropOperation(UItemProxyDragDropOperation::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->SceneToolSPtr = BasicProxyPtr;
				WidgetDragPtr->bIsInBackpakc = true;
				WidgetDragPtr->OnDrop.AddDynamic(this, &ThisClass::OnDroped);

				OutOperation = WidgetDragPtr;

				OnDragDelegate.ExcuteCallback(true, BasicProxyPtr);
			}
		}
	}
}

void UBackpackIcon::OnDroped(UDragDropOperation* Operation)
{
	OnDragDelegate.ExcuteCallback(false, nullptr);
}

void UBackpackIcon::SetItemType(FBasicProxy* InBasicProxyPtr)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FBackpackIcon::Get().Icon));
	if (ImagePtr)
	{
		if (InBasicProxyPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(
				InBasicProxyPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr, InBasicProxyPtr]()
				{
					ImagePtr->SetBrushFromTexture(InBasicProxyPtr->GetIcon().Get());
				}));
		}
		else
		{
			ImagePtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UBackpackIcon::OnAllocationCharacterProxyChanged(const TWeakPtr<FCharacterProxy>& AllocationCharacterProxyPtr)
{
	if (AllocationCharacterProxyPtr.IsValid())
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FBackpackIcon::Get().AllocationCharacterProxy));
		if (!UIPtr)
		{
			return;
		}

		UIPtr->SetVisibility(ESlateVisibility::Visible);
		if (AllocationCharacterProxyPtr.Pin()->Name.IsEmpty())
		{
			UIPtr->SetText(FText::FromString(AllocationCharacterProxyPtr.Pin()->Title));
		}
		else
		{
			UIPtr->SetText(FText::FromString(AllocationCharacterProxyPtr.Pin()->Name));
		}
	}
	else
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FBackpackIcon::Get().AllocationCharacterProxy));
		if (!UIPtr)
		{
			return;
		}

		UIPtr->SetVisibility(ESlateVisibility::Hidden);
	}
}
