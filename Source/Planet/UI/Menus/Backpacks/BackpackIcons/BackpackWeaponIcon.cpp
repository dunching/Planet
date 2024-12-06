
#include "BackpackWeaponIcon.h"

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
#include "ItemProxy_Minimal.h"
#include "GameplayTagsLibrary.h"

UBackpackWeaponIcon::UBackpackWeaponIcon(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{

}

void UBackpackWeaponIcon::InvokeReset(UUserWidget* BaseWidgetPtr)
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

void UBackpackWeaponIcon::ResetToolUIByData(const TSharedPtr<FBasicProxy>& InBasicUnitPtr)
{
	Super::ResetToolUIByData(InBasicUnitPtr);

	if (InBasicUnitPtr && InBasicUnitPtr->GetUnitType().MatchesTag(UGameplayTagsLibrary::Unit_Weapon))
	{
		UnitPtr = DynamicCastSharedPtr<FWeaponProxy>(InBasicUnitPtr);
	}
}

void UBackpackWeaponIcon::EnableIcon(bool bIsEnable)
{

}

void UBackpackWeaponIcon::SetNum(int32 NewNum)
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

void UBackpackWeaponIcon::SetValue(int32 Value)
{
	auto ProgressBarPtr = Cast<UProgressBar>(GetWidgetFromName(TEXT("ProgressBar")));
	if (!ProgressBarPtr)
	{
		return;
	}
}

void UBackpackWeaponIcon::ResetSize(const FVector2D& Size)
{
	auto SizeBoxPtr = Cast<USizeBox>(GetWidgetFromName(TEXT("IconSize")));
	if (SizeBoxPtr)
	{
		SizeBoxPtr->SetWidthOverride(Size.X);
		SizeBoxPtr->SetHeightOverride(Size.Y);
	}
}

void UBackpackWeaponIcon::NativeConstruct()
{
	Super::NativeConstruct();
}
