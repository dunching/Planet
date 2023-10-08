
#include "EquipIcon.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include <Kismet/GameplayStatics.h>

#include "EquipIconDrag.h"
#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "UI/Menu/Backpack/BackpackIcon.h"
#include "UI/Menu/Backpack/BackpackIconDrag.h"
#include <CacheAssetManager.h>
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"

UEquipIcon::UEquipIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UEquipIcon::ResetUIByData(const FItemNum& NewItemBase)
{
	ItemPropertyBase = NewItemBase;
	SetNum(NewItemBase.Num);
	SetItemType(NewItemBase);
}

void UEquipIcon::SetNum(int32 NewNum)
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

void UEquipIcon::SetItemType(FItemNum NewItemsType)
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
		if (NewItemsType.Num > 0)
		{
			ImagePtr->SetOpacity(1.f);
		}
		else
		{
			ImagePtr->SetOpacity(0.5f);
		}
	}
}

FItemNum UEquipIcon::GetItemsType() const
{
	return ItemPropertyBase;
}

int32 UEquipIcon::GetItemsNum() const
{
	return ItemPropertyBase.Num;
}

const FItemNum& UEquipIcon::GetItemPropertyBase() const
{
	return ItemPropertyBase;
}

void UEquipIcon::ResetSize(const FVector2D& Size)
{
	auto SizeBoxPtr = Cast<USizeBox>(GetWidgetFromName(TEXT("SizeBox")));
	if (SizeBoxPtr)
	{
		SizeBoxPtr->SetWidthOverride(Size.X);
		SizeBoxPtr->SetHeightOverride(Size.Y);
	}
}

void UEquipIcon::NativeConstruct()
{
	Super::NativeConstruct();

	ResetUIByData(ItemPropertyBase);
}

FReply UEquipIcon::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

bool UEquipIcon::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	if (InOperation->IsA(UEquipIconDrag::StaticClass()))
	{
 		auto WidgetDragPtr = Cast<UEquipIconDrag>(InOperation);
 		if (WidgetDragPtr)
 		{
 			ResetUIByData(WidgetDragPtr->ItemPropertyBase);
 		}
	}
	else if (InOperation->IsA(UBackpackIconDrag::StaticClass()))
	{
 		auto WidgetDragPtr = Cast<UBackpackIconDrag>(InOperation);
 		if (WidgetDragPtr)
 		{
 			ResetUIByData(WidgetDragPtr->ItemPropertyBase);
 		}
	}

	return true;
}

void UEquipIcon::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	auto BaseItemClassPtr = UAssetRefMap::GetInstance()->EquipIconClass;
	
	if (BaseItemClassPtr)
	{
		auto DragWidgetPtr = CreateWidget<UEquipIcon>(this, BaseItemClassPtr);
		if (DragWidgetPtr)
		{
			DragWidgetPtr->ResetSize(InGeometry.Size);
			DragWidgetPtr->ResetUIByData(ItemPropertyBase);

			auto WidgetDragPtr = Cast<UEquipIconDrag>(UWidgetBlueprintLibrary::CreateDragDropOperation(UEquipIconDrag::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->ItemPropertyBase = ItemPropertyBase;

				OutOperation = WidgetDragPtr;
			}
		}
	}
}