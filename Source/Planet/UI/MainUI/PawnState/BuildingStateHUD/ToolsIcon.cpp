
#include "ToolsIcon.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Containers/UnrealString.h"
#include <Kismet/GameplayStatics.h>
#include "Components/CanvasPanel.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Components/Border.h"
#include "Components/Overlay.h"

#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Engine/Texture2D.h"
#include "ToolsLibrary.h"
#include "BackpackIcon.h"

#include "StateTagExtendInfo.h"
#include "AssetRefMap.h"
#include "ItemsDragDropOperation.h"
#include "DragDropOperationWidget.h"
#include "GameplayTagsSubSystem.h"
#include "TemplateHelper.h"

namespace ToolsIcon
{
	const FName Content = TEXT("Content");

	const FName Number = TEXT("Number");

	const FName Enable = TEXT("Enable");

	const FName SizeBox = TEXT("SizeBox");

	const FName Icon = TEXT("Icon");

	const FName Overlay = TEXT("Overlay");
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
			OnResetUnit = NewPtr->OnResetUnit;
			ResetToolUIByData(NewPtr->UnitPtr);
		}
	}
}

void UToolIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr)
{
	if (BasicUnitPtr)
	{
		if (BasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Tool))
		{
			UnitPtr = DynamicCastSharedPtr<FToolProxy>(BasicUnitPtr);
		}
		else if (BasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables))
		{
			UnitPtr = DynamicCastSharedPtr<FConsumableProxy>(BasicUnitPtr);
		}
	}
	else
	{
		UnitPtr = nullptr;
	}

	OnResetUnit.ExcuteCallback(UnitPtr);

	auto UIPtr = Cast<UOverlay>(GetWidgetFromName(ToolsIcon::Overlay));
	if (UIPtr)
	{
		if (UnitPtr)
		{
			UIPtr->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			UIPtr->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	SetNum();
	SetItemType();
}

void UToolIcon::EnableIcon(bool bIsEnable)
{

}

TSharedPtr<FToolProxy> UToolIcon::GetToolUnit() const
{
	return DynamicCastSharedPtr<FToolProxy>(UnitPtr);
}

TSharedPtr<FConsumableProxy> UToolIcon::GetConsumablesUnit() const
{
	return DynamicCastSharedPtr<FConsumableProxy>(UnitPtr);
}

void UToolIcon::OnSublingIconReset(const TSharedPtr<FBasicProxy>& InToolUnitPtr)
{
	if (InToolUnitPtr && (InToolUnitPtr == UnitPtr))
	{
		ResetToolUIByData(nullptr);
	}
}

void UToolIcon::SetNum()
{
	int32 NewNum = 0;

	if (UnitPtr)
	{
		if (UnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Tool))
		{
			auto TempUnitPtr = DynamicCastSharedPtr<FToolProxy>(UnitPtr);
			if (TempUnitPtr)
			{
				NewNum = TempUnitPtr->GetNum();
			}
		}
		else if (UnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables))
		{
			auto TempUnitPtr = DynamicCastSharedPtr<FConsumableProxy>(UnitPtr);
			if (TempUnitPtr)
			{
				NewNum = TempUnitPtr->GetCurrentValue();
			}
		}
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
		if (UnitPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
			AsyncLoadTextureHandleAry.Add(StreamableManager.RequestAsyncLoad(UnitPtr->GetIcon().ToSoftObjectPath(), [this, ImagePtr]()
				{
					ImagePtr->SetBrushFromTexture(UnitPtr->GetIcon().Get());
				}));
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

bool UToolIcon::NativeOnDrop(
	const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation
)
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
