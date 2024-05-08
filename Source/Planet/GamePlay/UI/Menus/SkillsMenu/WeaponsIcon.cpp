
#include "WeaponsIcon.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include <Kismet/GameplayStatics.h>
#include "Components/CanvasPanel.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/Border.h"

#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "BackpackIcon.h"
#include <CacheAssetManager.h>
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"

UWeaponsIcon::UWeaponsIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UWeaponsIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
			ResetToolUIByData(NewPtr->WeaponUnitPtr);
			bIsInBackpakc = NewPtr->bIsInBackpakc;
		}
	}
}

void UWeaponsIcon::ResetToolUIByData(UBasicUnit * BasicUnitPtr)
{
	if (BasicUnitPtr && BasicUnitPtr->GetSceneToolsType() == ESceneToolsType::kWeapon)
	{
		WeaponUnitPtr = Cast<UWeaponUnit>(BasicUnitPtr);
		SetItemType();

		OnDroped.ExcuteCallback(WeaponUnitPtr);

		auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Default")));
		if (ImagePtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Collapsed);
		}
		auto BorderPtr = Cast<UBorder>(GetWidgetFromName(TEXT("Content")));
		if (BorderPtr)
		{
			BorderPtr->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Default")));
		if (ImagePtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);
		}
		auto BorderPtr = Cast<UBorder>(GetWidgetFromName(TEXT("Content")));
		if (BorderPtr)
		{
			BorderPtr->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UWeaponsIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Texture")));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandle = StreamableManager.RequestAsyncLoad(WeaponUnitPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
			{
				ImagePtr->SetBrushFromTexture(WeaponUnitPtr->GetIcon().Get());
			});
	}
}

void UWeaponsIcon::NativeConstruct()
{
	Super::NativeConstruct();

	ResetToolUIByData(nullptr);
}

FReply UWeaponsIcon::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		if (bIsInBackpakc)
		{
			Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

			return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
		}
		else
		{
			return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
		}
	}
	else if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		if (bIsInBackpakc)
		{
		}
		else
		{
			ResetToolUIByData(nullptr);
		}
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

bool UWeaponsIcon::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (InOperation->IsA(UItemsDragDropOperation::StaticClass()))
	{
		auto WidgetDragPtr = Cast<UItemsDragDropOperation>(InOperation);
		if (WidgetDragPtr)
		{
			ResetToolUIByData(WidgetDragPtr->SceneToolSPtr);
		}
	}

	return true;
}

void UWeaponsIcon::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	auto BaseItemClassPtr = UAssetRefMap::GetInstance()->DragDropOperationWidgetClass;

	if (BaseItemClassPtr)
	{
		auto DragWidgetPtr = CreateWidget<UDragDropOperationWidget>(this, BaseItemClassPtr);
		if (DragWidgetPtr)
		{
			DragWidgetPtr->ResetSize(InGeometry.Size);
			DragWidgetPtr->ResetToolUIByData(WeaponUnitPtr);

			auto WidgetDragPtr = Cast<UItemsDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemsDragDropOperation::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->SceneToolSPtr = WeaponUnitPtr;
				WidgetDragPtr->bIsInBackpakc = bIsInBackpakc;

				OutOperation = WidgetDragPtr;
			}
		}
	}
}