
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

namespace ToolsIcon
{
	const FName Content = TEXT("Content");

	const FName Number = TEXT("Number");

	const FName Enable = TEXT("Enable");

	const FName SizeBox = TEXT("SizeBox");

	const FName Icon = TEXT("Icon");
}

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
			ResetToolUIByData(NewPtr->ToolUnitPtr);
		}
	}
}

void UToolIcon::ResetToolUIByData(UBasicUnit * BasicUnitPtr)
{
	if (BasicUnitPtr && BasicUnitPtr->GetSceneToolsType() == ESceneToolsType::kTool)
	{
		ToolUnitPtr = Cast<UToolUnit>(BasicUnitPtr);
	}
	else
	{
		ToolUnitPtr = nullptr;
	}
	SetNum();
	SetItemType();
}

void UToolIcon::EnableIcon(bool bIsEnable)
{

}

UToolUnit* UToolIcon::GetToolUnit() const
{
	return ToolUnitPtr;
}

void UToolIcon::OnSublingIconReset(UToolUnit* InToolUnitPtr)
{
	if (InToolUnitPtr && (InToolUnitPtr == ToolUnitPtr))
	{
		ResetToolUIByData(nullptr);
	}
}

void UToolIcon::SetNum()
{
	int32 NewNum = 0;
	if (ToolUnitPtr)
	{
		NewNum = ToolUnitPtr->Num;
	}

	auto NumTextPtr = Cast<UTextBlock>(GetWidgetFromName(ToolsIcon::Number));
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
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(ToolsIcon::Icon));
	if (ImagePtr)
	{
		if (ToolUnitPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			AsyncLoadTextureHandle = StreamableManager.RequestAsyncLoad(ToolUnitPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
				{
					ImagePtr->SetBrushFromTexture(ToolUnitPtr->GetIcon().Get());
				});
		}
		else
		{
			ImagePtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UToolIcon::ResetSize(const FVector2D& Size)
{
	auto SizeBoxPtr = Cast<USizeBox>(GetWidgetFromName(ToolsIcon::SizeBox));
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
