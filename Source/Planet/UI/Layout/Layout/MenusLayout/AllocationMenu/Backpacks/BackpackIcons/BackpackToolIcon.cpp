
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
#include "ItemProxyDragDropOperation.h"
#include "ItemProxyDragDropOperationWidget.h"
#include "ItemProxy_Minimal.h"
#include "GameplayTagsLibrary.h"

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
		}
	}
}

void UBackpackToolIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& InBasicProxyPtr)
{
	Super::ResetToolUIByData(InBasicProxyPtr);

	if (InBasicProxyPtr && InBasicProxyPtr->GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Tool))
	{
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
