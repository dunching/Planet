
#include "MyBaseProperty.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"

namespace MyBaseProperty
{
	const FName ProgressBar = TEXT("ProgressBar");

	const FName Text = TEXT("Text");
}

void UMyBaseProperty::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMyBaseProperty::NativeDestruct()
{
	if (OnValueChanged)
	{
		OnValueChanged->UnBindCallback();
	}

	Super::NativeDestruct();
}

void UMyBaseProperty::SetDataSource(FBasePropertySet& Property)
{
	SetCurrentValue(Property.GetCurrentValue());
	OnValueChanged = Property.AddOnValueChanged(
		std::bind(&ThisClass::SetCurrentValue, this, std::placeholders::_2)
	);
}

void UMyBaseProperty::SetDataSource(FBasePropertySet& Property1, FBasePropertySet& Property2)
{
	SetCurrentValue1(Property1.GetCurrentValue());
	OnValueChanged = Property1.AddOnValueChanged(
		std::bind(&ThisClass::SetCurrentValue1, this, std::placeholders::_2)
	);
	SetCurrentValue2(Property2.GetCurrentValue());
	OnValueChanged = Property2.AddOnValueChanged(
		std::bind(&ThisClass::SetCurrentValue2, this, std::placeholders::_2)
	);
}

void UMyBaseProperty::SetCurrentValue(int32 InCurrentValue)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(MyBaseProperty::Text));
	if (UIPtr)
	{
		UIPtr->SetText(FText::FromString(*FString::Printf(TEXT("%d"), InCurrentValue)));
	}
}

void UMyBaseProperty::SetCurrentValue1(int32 InCurrentValue)
{
	Value1 = InCurrentValue;
	ValueChanged();
}

void UMyBaseProperty::SetCurrentValue2(int32 InCurrentValue)
{
	Value2 = InCurrentValue;
	ValueChanged();
}

void UMyBaseProperty::ValueChanged()
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(MyBaseProperty::Text));
	if (UIPtr)
	{
		UIPtr->SetText(FText::FromString(*FString::Printf(TEXT("%d/%d%%"), Value1, Value2)));
	}
}
