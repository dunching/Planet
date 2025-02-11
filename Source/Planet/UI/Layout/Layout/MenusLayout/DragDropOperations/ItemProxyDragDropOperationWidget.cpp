
#include "ItemProxyDragDropOperationWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include <Kismet/GameplayStatics.h>
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "BackpackIcon.h"

#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemProxyDragDropOperation.h"
#include "ItemProxy_Minimal.h"

UItemProxyDragDropOperationWidget::UItemProxyDragDropOperationWidget(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void UItemProxyDragDropOperationWidget::InvokeReset(UUserWidget* BaseWidgetPtr)
{
}

void UItemProxyDragDropOperationWidget::ResetToolUIByData(const TSharedPtr<FBasicProxy>& InBasicProxyPtr)
{
	if (InBasicProxyPtr)
	{
		BasicProxyPtr = InBasicProxyPtr;
		SetItemType(BasicProxyPtr.Get());
	}
}

void UItemProxyDragDropOperationWidget::EnableIcon(bool bIsEnable)
{
}

void UItemProxyDragDropOperationWidget::SetNum(int32 NewNum)
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

void UItemProxyDragDropOperationWidget::SetItemType(FBasicProxy* ToolSPtr)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Texture")));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(
			ToolSPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr, ToolSPtr]()
			{
				ImagePtr->SetBrushFromTexture(ToolSPtr->GetIcon().Get());
			}));
	}
}

void UItemProxyDragDropOperationWidget::ResetSize(const FVector2D& Size)
{
	auto SizeBoxPtr = Cast<USizeBox>(GetWidgetFromName(TEXT("SizeBox")));
	if (SizeBoxPtr)
	{
		SizeBoxPtr->SetWidthOverride(Size.X);
		SizeBoxPtr->SetHeightOverride(Size.Y);
	}
}

UAllocationableProxyDragDropOperationWidget::UAllocationableProxyDragDropOperationWidget(
	const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void UAllocationableProxyDragDropOperationWidget::InvokeReset(UUserWidget* BaseWidgetPtr)
{
}

void UAllocationableProxyDragDropOperationWidget::ResetToolUIByData(
	const TSharedPtr<FBasicProxy>& InBasicProxyPtr)
{
	if (InBasicProxyPtr)
	{
		BasicProxyPtr = DynamicCastSharedPtr<FAllocationbleProxy>(InBasicProxyPtr);
		SetItemType(BasicProxyPtr.Get());
	}
}

void UAllocationableProxyDragDropOperationWidget::EnableIcon(bool bIsEnable)
{
}

void UAllocationableProxyDragDropOperationWidget::SetNum(int32 NewNum)
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

void UAllocationableProxyDragDropOperationWidget::SetItemType(FBasicProxy* ToolSPtr)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Texture")));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(
			ToolSPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr, ToolSPtr]()
			{
				ImagePtr->SetBrushFromTexture(ToolSPtr->GetIcon().Get());
			}));
	}
}

void UAllocationableProxyDragDropOperationWidget::ResetSize(const FVector2D& Size)
{
	auto SizeBoxPtr = Cast<USizeBox>(GetWidgetFromName(TEXT("SizeBox")));
	if (SizeBoxPtr)
	{
		SizeBoxPtr->SetWidthOverride(Size.X);
		SizeBoxPtr->SetHeightOverride(Size.Y);
	}
}
