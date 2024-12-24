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
	).AddUObject(this, &ThisClass::OnValueChanged);
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
	).AddUObject(this, &ThisClass::OnValue1Changed);
	Value1 = Value;

	AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
		MaxAttribute
	).AddUObject(this, &ThisClass::OnValue2Changed);
	Value2 = MaxValue;

	SetCurrentValue(Value1, Value2);
}

void UMyBaseProperty::OnValueChanged(const FOnAttributeChangeData& CurrentValue)
{
	SetCurrentValue(CurrentValue.NewValue);
}

void UMyBaseProperty::SetCurrentValue(int32 InCurrentValue)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(MyBaseProperty::Text));
	if (UIPtr)
	{
		UIPtr->SetText(FText::FromString(*FString::Printf(TEXT("%d"), InCurrentValue)));
	}
}

void UMyBaseProperty::OnValue1Changed(const FOnAttributeChangeData& CurrentValue)
{
	Value1 = CurrentValue.NewValue;

	SetCurrentValue(Value1, Value2);
}

void UMyBaseProperty::OnValue2Changed(const FOnAttributeChangeData& CurrentValue)
{
	Value2 = CurrentValue.NewValue;

	SetCurrentValue(Value1, Value2);
}

void UMyBaseProperty::SetCurrentValue(int32 InValue1, int32 InValue2)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(MyBaseProperty::Text));
	if (UIPtr)
	{
		UIPtr->SetText(FText::FromString(*FString::Printf(TEXT("%d/%d%%"), Value1, Value2)));
	}
}
