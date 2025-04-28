
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

void UMyProgressBar::SetDataSource(
	UAbilitySystemComponent*AbilitySystemComponentPtr,
	FGameplayAttribute Attribute,
	float Value,
	FGameplayAttribute MaxAttribute,
	float InMaxValue
		)
{
	AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
	Attribute
		).AddUObject(this, &ThisClass::SetCurrentValue_Re);
	CurrentValue = Value;

	AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
	MaxAttribute
		).AddUObject(this, &ThisClass::SetMaxValue_Re);
	MaxValue = InMaxValue;

	ValueChanged();
}

void UMyProgressBar::SetCurrentValue(int32 InCurrentValue)
{
	CurrentValue = InCurrentValue;

	ValueChanged();
}

void UMyProgressBar::SetCurrentValue_Re(const FOnAttributeChangeData& InCurrentValue)
{
	CurrentValue = InCurrentValue.NewValue;

	ValueChanged();
}

void UMyProgressBar::SetMaxValue(int32 InMaxValue)
{
	MaxValue = InMaxValue;

	ValueChanged();
}

void UMyProgressBar::SetMaxValue_Re(const FOnAttributeChangeData& InMaxValue)
{
	MaxValue = InMaxValue.NewValue;

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
