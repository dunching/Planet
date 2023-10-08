
#include "BackpackIcon.h"

#include <Kismet/GameplayStatics.h>
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include "BackpackIconDrag.h"
#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"

#include <CacheAssetManager.h>
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"

UBackpackIcon::UBackpackIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UBackpackIcon::ResetUIByData(const FItemNum& NewItemBase)
{
	ItemPropertyBase = NewItemBase;
	SetNum(NewItemBase.Num);
	SetItemType(NewItemBase);
}

void UBackpackIcon::SetNum(int32 NewNum)
{
	ItemPropertyBase.Num = NewNum;

	auto NumTextPtr = Cast<UTextBlock>(GetWidgetFromName(TEXT("Number")));
	if (!NumTextPtr)
	{
		return;
	}
	if (ItemPropertyBase.Num > 0)
	{
		const auto NumStr = FString::Printf(TEXT("%d"), ItemPropertyBase.Num);

		NumTextPtr->SetText(FText::FromString(NumStr));
	}
	else
	{
		NumTextPtr->SetText(FText::FromString(TEXT("")));
	}
}

void UBackpackIcon::SetItemType(FItemNum NewItemsType)
{
	ItemPropertyBase = NewItemsType;

	auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Texture")));
	if (ImagePtr)
	{
		auto CacheAssetManagerPtr = UCacheAssetManager::GetInstance();
		
		CacheAssetManagerPtr->GetTextureByItemType(ItemPropertyBase, [ImagePtr](auto TexturePtr)
			{
			ImagePtr->SetBrushFromTexture(TexturePtr);
			});
	}
}

FItemNum UBackpackIcon::GetItemsType() const
{
	return ItemPropertyBase;
}

int32 UBackpackIcon::GetItemsNum() const
{
	return ItemPropertyBase.Num;
}

const FItemNum& UBackpackIcon::GetItemPropertyBase() const
{
	return ItemPropertyBase;
}

void UBackpackIcon::ResetSize(const FVector2D& Size)
{
	auto SizeBoxPtr = Cast<USizeBox>(GetWidgetFromName(TEXT("IconSize")));
	if (SizeBoxPtr)
	{
		SizeBoxPtr->SetWidthOverride(Size.X);
		SizeBoxPtr->SetHeightOverride(Size.Y);
	}
}

void UBackpackIcon::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply UBackpackIcon::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

bool UBackpackIcon::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	auto WidgetDragPtr = Cast<UBackpackIconDrag>(InOperation);
	if (WidgetDragPtr)
	{
		ResetUIByData(WidgetDragPtr->ItemPropertyBase);
	}

	return true;
}

void UBackpackIcon::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	auto BaseItemClassPtr = UAssetRefMap::GetInstance()->BackpackIconClass;

	if (BaseItemClassPtr)
	{
		auto DragWidgetPtr = CreateWidget<UBackpackIcon>(this, BaseItemClassPtr);
		if (DragWidgetPtr)
		{
			DragWidgetPtr->ResetSize(InGeometry.Size);
			DragWidgetPtr->ResetUIByData(ItemPropertyBase);

			auto WidgetDragPtr = Cast<UBackpackIconDrag>(UWidgetBlueprintLibrary::CreateDragDropOperation(UBackpackIconDrag::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->ItemPropertyBase = ItemPropertyBase;

				OutOperation = WidgetDragPtr;
			}
		}
	}
}