
#include "BackpackMaterialIcon.h"

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
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "ItemProxy_Minimal.h"
#include "GameplayTagsLibrary.h"
#include "VisitorSubsystem.h"
#include "AllocationSkillsMenu.h"

UBackpackMaterialIcon::UBackpackMaterialIcon(const FObjectInitializer& ObjectInitializer) :
                                                                                    Super(ObjectInitializer)
{

}

FReply UBackpackMaterialIcon::NativeOnMouseButtonDown(
	const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent
	)
{
	if (BasicProxyPtr)
	{
		if (BasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Material))
		{
			AllocationSkillsMenuPtr->OnSelectedProxy(BasicProxyPtr);
		}
	}

	return FReply::Handled();
}

void UBackpackMaterialIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
{
	if (BaseWidgetPtr)
	{
		auto NewPtr = Cast<ThisClass>(BaseWidgetPtr);
		if (NewPtr)
		{
			ResetToolUIByData(NewPtr->ProxyPtr);
		}
	}
}

void UBackpackMaterialIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& InBasicProxyPtr)
{
	Super::ResetToolUIByData(InBasicProxyPtr);

	if (InBasicProxyPtr)
	{
		if (
			InBasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Material) 
			)
		{
			ProxyPtr = DynamicCastSharedPtr<FProxyType>(InBasicProxyPtr);
			SetNum(ProxyPtr->GetNum());
		}
	}
}

void UBackpackMaterialIcon::OnAllocationCharacterProxyChanged(
	const TWeakPtr<FCharacterProxy>& AllocationCharacterProxyPtr
	)
{
	// 无此项
}

void UBackpackMaterialIcon::EnableIcon(bool bIsEnable)
{

}

void UBackpackMaterialIcon::SetNum(int32 NewNum)
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

void UBackpackMaterialIcon::SetValue(int32 Value)
{
	auto ProgressBarPtr = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar")));
	if (!ProgressBarPtr)
	{
		return;
	}
}

void UBackpackMaterialIcon::ResetSize(const FVector2D& Size)
{
	auto SizeBoxPtr = Cast<USizeBox>(GetWidgetFromName(TEXT("IconSize")));
	if (SizeBoxPtr)
	{
		SizeBoxPtr->SetWidthOverride(Size.X);
		SizeBoxPtr->SetHeightOverride(Size.Y);
	}
}

void UBackpackMaterialIcon::NativeConstruct()
{
	Super::NativeConstruct();
}
