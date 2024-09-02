
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
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"
#include "SceneElement.h"
#include "GameplayTagsSubSystem.h"
#include "UICommon.h"
#include "CharacterAttibutes.h"

struct FBackpackIcon : public TStructVariable<FBackpackIcon>
{
	const FName Content = TEXT("Content");

	const FName Enable = TEXT("Enable");

	const FName Icon = TEXT("Icon");

	const FName AllocationCharacterUnit = TEXT("AllocationCharacterUnit");
};

UBackpackIcon::UBackpackIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UBackpackIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
{
}

void UBackpackIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& InBasicUnitPtr)
{
	BasicUnitPtr = InBasicUnitPtr;

	if (BasicUnitPtr)
	{
		SetItemType(BasicUnitPtr.Get());

		OnAllocationCharacterUnitChangedHandle = BasicUnitPtr->OnAllocationCharacterUnitChanged.AddCallback(
			std::bind(&ThisClass::OnAllocationCharacterUnitChanged, this, std::placeholders::_1)
		);

		OnAllocationCharacterUnitChanged(InBasicUnitPtr->GetAllocationCharacterUnit());
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

void UBackpackIcon::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
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
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->SceneToolSPtr = BasicUnitPtr;
				WidgetDragPtr->bIsInBackpakc = true;
				WidgetDragPtr->OnDrop.AddDynamic(this, &ThisClass::OnDroped);

				OutOperation = WidgetDragPtr;

				OnDragDelegate.ExcuteCallback(true, BasicUnitPtr);
			}
		}
	}
}

void UBackpackIcon::OnDroped(UDragDropOperation* Operation)
{
	OnDragDelegate.ExcuteCallback(false, nullptr);
}

void UBackpackIcon::SetItemType(FBasicProxy* InBasicUnitPtr)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(FBackpackIcon::Get().Icon));
	if (ImagePtr)
	{
		if (InBasicUnitPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(InBasicUnitPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr, InBasicUnitPtr]()
				{
					ImagePtr->SetBrushFromTexture(InBasicUnitPtr->GetIcon().Get());
				}));
		}
		else
		{
			ImagePtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UBackpackIcon::OnAllocationCharacterUnitChanged(const TSharedPtr<FCharacterProxy>& AllocationCharacterUnitPtr)
{
	if (AllocationCharacterUnitPtr)
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FBackpackIcon::Get().AllocationCharacterUnit));
		if (!UIPtr)
		{
			return;
		}

		UIPtr->SetVisibility(ESlateVisibility::Visible);
		UIPtr->SetText(FText::FromName(AllocationCharacterUnitPtr->CharacterAttributesSPtr->Name));
	}
	else
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(FBackpackIcon::Get().AllocationCharacterUnit));
		if (!UIPtr)
		{
			return;
		}

		UIPtr->SetVisibility(ESlateVisibility::Hidden);
	}
}
