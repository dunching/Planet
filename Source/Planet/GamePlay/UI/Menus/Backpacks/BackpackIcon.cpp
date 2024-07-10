
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

#include <CacheAssetManager.h>
#include <AssetRefrencePath.h>
#include "AssetRefMap.h"
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"
#include "SceneElement.h"

UBackpackIcon::UBackpackIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UBackpackIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
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

void UBackpackIcon::ResetToolUIByData(UBasicUnit * BasicUnitPtr)
{
	if (BasicUnitPtr && BasicUnitPtr->GetSceneToolsType() == ESceneToolsType::kTool)
	{
		ToolUnitPtr = Cast<UToolUnit>(BasicUnitPtr);
		SetNum(ToolUnitPtr->Num);
		SetItemType();
	}
}

void UBackpackIcon::EnableIcon(bool bIsEnable)
{

}

void UBackpackIcon::SetNum(int32 NewNum)
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

void UBackpackIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Texture")));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandle = StreamableManager.RequestAsyncLoad(ToolUnitPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
			{
				ImagePtr->SetBrushFromTexture(ToolUnitPtr->GetIcon().Get());
			});
	}
}

void UBackpackIcon::SetValue(int32 Value)
{
	auto ProgressBarPtr = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar")));
	if (!ProgressBarPtr)
	{
		return;
	}
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

void UBackpackIcon::NativeOnDragDetected(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation
)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	auto BaseItemClassPtr = UAssetRefMap::GetInstance()->DragDropOperationWidgetClass;

	if (BaseItemClassPtr)
	{
		auto DragWidgetPtr = CreateWidget<UDragDropOperationWidget>(this, BaseItemClassPtr);
		if (DragWidgetPtr)
		{
			DragWidgetPtr->ResetSize(InGeometry.Size);
			DragWidgetPtr->ResetToolUIByData(ToolUnitPtr);

			auto WidgetDragPtr = Cast<UItemsDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemsDragDropOperation::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->SceneToolSPtr = ToolUnitPtr;

				OutOperation = WidgetDragPtr;
			}
		}
	}
}