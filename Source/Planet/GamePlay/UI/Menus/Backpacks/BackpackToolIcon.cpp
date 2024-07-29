
#include "BackpackToolIcon.h"

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

UBackpackToolIcon::UBackpackToolIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UBackpackToolIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
			ResetToolUIByData(NewPtr->UnitPtr);
		}
	}
}

void UBackpackToolIcon::ResetToolUIByData(UBasicUnit* BasicUnitPtr)
{
	if (BasicUnitPtr && BasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Tool))
	{
		UnitPtr = Cast<UToolUnit>(BasicUnitPtr);
		SetNum(UnitPtr->GetNum());
		SetItemType();
	}
}

void UBackpackToolIcon::EnableIcon(bool bIsEnable)
{

}

void UBackpackToolIcon::SetNum(int32 NewNum)
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

void UBackpackToolIcon::SetItemType()
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(TEXT("Texture")));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(UnitPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
			{
				ImagePtr->SetBrushFromTexture(UnitPtr->GetIcon().Get());
			}));
	}
}

void UBackpackToolIcon::SetValue(int32 Value)
{
	auto ProgressBarPtr = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar")));
	if (!ProgressBarPtr)
	{
		return;
	}
}

void UBackpackToolIcon::ResetSize(const FVector2D& Size)
{
	auto SizeBoxPtr = Cast<USizeBox>(GetWidgetFromName(TEXT("IconSize")));
	if (SizeBoxPtr)
	{
		SizeBoxPtr->SetWidthOverride(Size.X);
		SizeBoxPtr->SetHeightOverride(Size.Y);
	}
}

void UBackpackToolIcon::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply UBackpackToolIcon::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

void UBackpackToolIcon::NativeOnDragDetected(
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
			DragWidgetPtr->ResetToolUIByData(UnitPtr);

			auto WidgetDragPtr = Cast<UItemsDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UItemsDragDropOperation::StaticClass()));
			if (WidgetDragPtr)
			{
				WidgetDragPtr->DefaultDragVisual = DragWidgetPtr;
				WidgetDragPtr->SceneToolSPtr = UnitPtr;

				OutOperation = WidgetDragPtr;
			}
		}
	}
}