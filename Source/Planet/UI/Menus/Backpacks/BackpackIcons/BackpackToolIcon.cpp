
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
#include "ItemProxy.h"
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

void UBackpackToolIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& InBasicUnitPtr)
{
	Super::ResetToolUIByData(InBasicUnitPtr);

	if (InBasicUnitPtr && InBasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::Unit_Tool))
	{
		UnitPtr = DynamicCastSharedPtr<FToolProxy>(InBasicUnitPtr);
		SetNum(UnitPtr->GetNum());
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
