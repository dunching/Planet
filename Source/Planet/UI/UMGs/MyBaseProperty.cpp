
#include "MyBaseProperty.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"

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

void UMyBaseProperty::SetDataSource(
	UAbilitySystemComponent* AbilitySystemComponentPtr,
	FGameplayAttribute Attribute,
	float Value
	)
{
	AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
	Attribute
		).AddUObject(this, &ThisClass::SetCurrentValue_Re);
	SetCurrentValue(Value);
}

void UMyBaseProperty::SetDataSource(
	UAbilitySystemComponent* AbilitySystemComponentPtr,
	FGameplayAttribute Attribute,
	float Value,
	FGameplayAttribute MaxAttribute,
	float MaxValue
	)
{
	AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
	Attribute
		).AddUObject(this, &ThisClass::SetCurrentValue1_Re);
	Value1 = Value;

	AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
	MaxAttribute
		).AddUObject(this, &ThisClass::SetCurrentValue2_Re);
	Value2 = MaxValue;

	ValueChanged();
}

void UMyBaseProperty::SetCurrentValue_Re(const FOnAttributeChangeData& CurrentValue)
{
}

void UMyBaseProperty::SetCurrentValue(int32 InCurrentValue)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(MyBaseProperty::Text));
	if (UIPtr)
	{
		UIPtr->SetText(FText::FromString(*FString::Printf(TEXT("%d"), InCurrentValue)));
	}
}

void UMyBaseProperty::SetCurrentValue1_Re(const FOnAttributeChangeData& CurrentValue)
{
}

void UMyBaseProperty::SetCurrentValue1(int32 InCurrentValue)
{
	Value1 = InCurrentValue;
	ValueChanged();
}

void UMyBaseProperty::SetCurrentValue2_Re(const FOnAttributeChangeData& CurrentValue)
{
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
