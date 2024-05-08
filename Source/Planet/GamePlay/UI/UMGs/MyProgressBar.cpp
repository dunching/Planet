
#include "MyProgressBar.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "CharacterBase.h"

namespace MyProgressBar
{
	const FName ProgressBar = TEXT("ProgressBar");

	const FName Text = TEXT("Text");
}

void UMyProgressBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMyProgressBar::NativeDestruct()
{
	if (OnMaxPPValueChanged)
	{
		OnMaxPPValueChanged->UnBindCallback();
	}
	if (OnPPValueChanged)
	{
		OnPPValueChanged->UnBindCallback();
	}

	Super::NativeDestruct();
}

void UMyProgressBar::SetDataSource(FBasePropertySet& Property)
{
	{
		SetMaxValue(Property.GetMaxValue());
		OnMaxPPValueChanged = Property.GetMaxProperty().CallbackContainerHelper.AddOnValueChanged(
			std::bind(&ThisClass::SetMaxValue, this, std::placeholders::_2)
		);
	}
	{
		SetCurrentValue(Property.GetCurrentValue());
		OnPPValueChanged = Property.GetCurrentProperty().CallbackContainerHelper.AddOnValueChanged(
			std::bind(&ThisClass::SetCurrentValue, this, std::placeholders::_2)
		);
	}
}

void UMyProgressBar::SetCurrentValue(int32 InCurrentValue)
{
	CurrentValue = InCurrentValue;

	ValueChanged();
}

void UMyProgressBar::SetMaxValue(int32 InMaxValue)
{
	MaxValue = InMaxValue;

	ValueChanged();
}

void UMyProgressBar::ValueChanged()
{
	{
		auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(MyProgressBar::Text));
		if (UIPtr)
		{
			UIPtr->SetText(FText::FromString(*FString::Printf(TEXT("%d/%d"), CurrentValue, MaxValue)));
		}
	}
	const auto Percent = static_cast<float>(CurrentValue) / MaxValue;
	{
		auto UIPtr = Cast<UProgressBar>(GetWidgetFromName(MyProgressBar::ProgressBar));
		if (UIPtr)
		{
			UIPtr->SetPercent(Percent);
		}
	}
}
