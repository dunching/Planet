
#include "ToolsIcon.h"

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

UToolIcon::UToolIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UToolIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
			ResetToolUIByData(NewPtr->ToolSPtr);
		}
	}
}

void UToolIcon::ResetToolUIByData(UBasicUnit * BasicUnitPtr)
{
	if (BasicUnitPtr && BasicUnitPtr->GetSceneToolsType() == ESceneToolsType::kTool)
	{
		ToolSPtr = Cast<UToolUnit>(BasicUnitPtr);
		SetNum(ToolSPtr->Num);
		SetItemType();

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

UToolUnit* UToolIcon::GetToolUnit() const
{
	return ToolSPtr;
}

void UToolIcon::SetNum(int32 NewNum)
{
	auto NumTextPtr = Cast<UTextBlock>(GetWidgetFromName(TEXT("Number")));
	if (!NumTextPtr)
	{
		return;
	}
	if (NewNum > 0)
	{
		const auto NumStr = FString::Printf(TEXT("%d"), NewNum);

		NumTextPtr->SetText(FText::FromString(NumStr));
	}
	else
	{
		NumTextPtr->SetText(FText::FromString(TEXT("")));
	}
}

void UToolIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Texture")));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandle = StreamableManager.RequestAsyncLoad(ToolSPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
			{
				ImagePtr->SetBrushFromTexture(ToolSPtr->GetIcon().Get());
			});
	}
}

void UToolIcon::ResetSize(const FVector2D& Size)
{
	auto SizeBoxPtr = Cast<USizeBox>(GetWidgetFromName(TEXT("SizeBox")));
	if (SizeBoxPtr)
	{
		SizeBoxPtr->SetWidthOverride(Size.X);
		SizeBoxPtr->SetHeightOverride(Size.Y);
	}
}

void UToolIcon::NativeConstruct()
{
	Super::NativeConstruct();

	ResetToolUIByData(nullptr);
}

FReply UToolIcon::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

bool UToolIcon::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
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
