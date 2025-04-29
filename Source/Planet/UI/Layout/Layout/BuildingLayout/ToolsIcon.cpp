
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
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "GameplayTagsLibrary.h"
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
			OnResetProxy = NewPtr->OnResetProxy;
			ResetToolUIByData(NewPtr->ProxyPtr);
		}
	}
}

void UToolIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)
{
	if (BasicProxyPtr)
	{
		if (BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
		{
			ProxyPtr = DynamicCastSharedPtr<FToolProxy>(BasicProxyPtr);
		}
		else if (BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
		{
			ProxyPtr = DynamicCastSharedPtr<FConsumableProxy>(BasicProxyPtr);
		}
	}
	else
	{
		ProxyPtr = nullptr;
	}

	OnResetProxy.ExcuteCallback(ProxyPtr);

	auto UIPtr = Cast<UOverlay>(GetWidgetFromName(ToolsIcon::Overlay));
	if (UIPtr)
	{
		if (ProxyPtr)
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

TSharedPtr<FToolProxy> UToolIcon::GetToolProxy() const
{
	return DynamicCastSharedPtr<FToolProxy>(ProxyPtr);
}

TSharedPtr<FConsumableProxy> UToolIcon::GetConsumablesProxy() const
{
	return DynamicCastSharedPtr<FConsumableProxy>(ProxyPtr);
}

void UToolIcon::OnSublingIconReset(const TSharedPtr<FBasicProxy>& InToolProxyPtr)
{
	if (InToolProxyPtr && (InToolProxyPtr == ProxyPtr))
	{
		ResetToolUIByData(nullptr);
	}
}

void UToolIcon::SetNum()
{
	int32 NewNum = 0;

	if (ProxyPtr)
	{
		if (ProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
		{
			auto TempProxyPtr = DynamicCastSharedPtr<FToolProxy>(ProxyPtr);
			if (TempProxyPtr)
			{
				NewNum = TempProxyPtr->GetNum();
			}
		}
		else if (ProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Consumables))
		{
			auto TempProxyPtr = DynamicCastSharedPtr<FConsumableProxy>(ProxyPtr);
			if (TempProxyPtr)
			{
				NewNum = TempProxyPtr->GetCurrentValue();
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
		if (ProxyPtr)
		{
			ImagePtr->SetVisibility(ESlateVisibility::Visible);

			AsyncLoadText(ProxyPtr->GetIcon(),ImagePtr );
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

	if (InOperation->IsA(UItemProxyDragDropOperation::StaticClass()))
	{
 		auto WidgetDragPtr = Cast<UItemProxyDragDropOperation>(InOperation);
 		if (WidgetDragPtr)
		{
			ResetToolUIByData(WidgetDragPtr->SceneToolSPtr);
 		}
	}

	return true;
}
