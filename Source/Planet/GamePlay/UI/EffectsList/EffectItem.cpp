
#include "EffectItem.h"

#include "Engine/AssetManager.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

namespace EffectItem
{
	const FName Percent = TEXT("Percent");

	const FName ProgressBar = TEXT("ProgressBar");

	const FName Text = TEXT("Text");

	const FName Icon = TEXT("Icon");

	const FName Inversion = TEXT("Inversion");

	const FName TextCanvas = TEXT("TextCanvas");
}

void UEffectItem::SetNum(int32 NewNum)
{
	SetSetNumIsDisplay(true);
	auto NumTextPtr = Cast<UTextBlock>(GetWidgetFromName(EffectItem::Text));
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

void UEffectItem::SetSetNumIsDisplay(bool bIsDisplay)
{
	{
		auto UIPtr = GetWidgetFromName(EffectItem::TextCanvas);
		if (!UIPtr)
		{
			return;
		}
		UIPtr->SetVisibility(bIsDisplay ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UEffectItem::SetPercent(bool bIsInversion, float Percent)
{
	SetPercentIsDisplay(true);
	auto UIPtr = Cast<UImage>(GetWidgetFromName(EffectItem::ProgressBar));
	if (!UIPtr)
	{
		return;
	}
	auto MIDPtr = UIPtr->GetDynamicMaterial();
	if (MIDPtr)
	{
		MIDPtr->SetScalarParameterValue(EffectItem::Percent, Percent);
		MIDPtr->SetScalarParameterValue(EffectItem::Inversion, bIsInversion);
	}
}

void UEffectItem::SetPercentIsDisplay(bool bIsDisplay)
{
	{
		auto UIPtr = GetWidgetFromName(EffectItem::ProgressBar);
		if (!UIPtr)
		{
			return;
		}
		UIPtr->SetVisibility(bIsDisplay ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}

void UEffectItem::SetTexutre(const TSoftObjectPtr<UTexture2D>& TexturePtr)
{
	auto ImagePtr = Cast<UImage>(GetWidgetFromName(EffectItem::Icon));
	if (ImagePtr)
	{
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandle = StreamableManager.RequestAsyncLoad(TexturePtr.ToSoftObjectPath(), [this, ImagePtr, TexturePtr]()
			{
				ImagePtr->SetBrushFromTexture(TexturePtr.Get());
			});
	}
}

void UEffectItem::NativeConstruct()
{
	Super::NativeConstruct();

	SetSetNumIsDisplay(false);
	SetPercentIsDisplay(false);
}

void UEffectItem::NativeDestruct()
{
	Super::NativeDestruct();
}
