
#include "PlaceWidgetBase.h"

#include "Components/TextBlock.h"

void UPlaceWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	DisablePromt();
}

void UPlaceWidgetBase::SetPromtStr(const FString& PromPtStr)
{
	auto BorderPtr = GetWidgetFromName(BorderName);
	if (BorderPtr)
	{
		BorderPtr->SetVisibility(ESlateVisibility::Visible);
	}

	auto TextPtr = Cast<UTextBlock>(GetWidgetFromName(TextName));
	if (TextPtr)
	{
		TextPtr->SetText(FText::FromString(PromPtStr));
	}
}

void UPlaceWidgetBase::DisablePromt()
{
	auto BorderPtr = GetWidgetFromName(BorderName);
	if (BorderPtr)
	{
		BorderPtr->SetVisibility(ESlateVisibility::Hidden);
	}
}

