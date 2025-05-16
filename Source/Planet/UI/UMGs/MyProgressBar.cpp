
#include "MyProgressBar.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "CharacterBase.h"

namespace MyProgressBar
{
	const FName ProgressBar = TEXT("ProgressBar");
	
	const FName ProgressBar_Overlay = TEXT("ProgressBar_Overlay");
	
	const FName Text = TEXT("Text");
}

void UMyProgressBar::NativeConstruct()
{
	Super::NativeConstruct();

	ValueChanged();
	OverlayValueChanged();
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

void UMyProgressBar::SetOverlayDataSource(
	UAbilitySystemComponent* AbilitySystemComponentPtr,
	FGameplayAttribute Attribute,
	float Value
	)
{
	AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
	Attribute
		).AddUObject(this, &ThisClass::SetOverlayValue);
	OverlayValue = Value;

}

void UMyProgressBar::SetCurrentValue(int32 InCurrentValue)
{
	CurrentValue = InCurrentValue;

	ValueChanged();
}

void UMyProgressBar::SetCurrentValue_Re(const FOnAttributeChangeData& AttributeChangeData)
{
	CurrentValue = AttributeChangeData.NewValue;

	ValueChanged();
}

void UMyProgressBar::SetMaxValue(int32 InMaxValue)
{
	MaxValue = InMaxValue;

	ValueChanged();
}

void UMyProgressBar::SetMaxValue_Re(const FOnAttributeChangeData& AttributeChangeData)
{
	MaxValue = AttributeChangeData.NewValue;

	ValueChanged();
}

void UMyProgressBar::SetOverlayValue(
	const FOnAttributeChangeData& AttributeChangeData
	)
{
	OverlayValue = AttributeChangeData.NewValue;

	OverlayValueChanged();
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

	if (MaxValue > 0)
	{
		const auto Percent = static_cast<float>(CurrentValue) / MaxValue;
		{
			auto UIPtr = Cast<UProgressBar>(GetWidgetFromName(MyProgressBar::ProgressBar));
			if (UIPtr)
			{
				UIPtr->SetPercent(Percent);
			}
		}
	}
	else
	{
		auto UIPtr = Cast<UProgressBar>(GetWidgetFromName(MyProgressBar::ProgressBar));
		if (UIPtr)
		{
			UIPtr->SetPercent(0);
		}
	}
}

void UMyProgressBar::OverlayValueChanged()
{
	if (MaxValue > 0)
	{
		const auto Percent = FMath::Clamp( static_cast<float>(OverlayValue) / MaxValue,0,1);
		{
			auto UIPtr = Cast<UProgressBar>(GetWidgetFromName(MyProgressBar::ProgressBar_Overlay));
			if (UIPtr)
			{
				UIPtr->SetPercent(Percent);
			}
		}
	}
	else
	{
		auto UIPtr = Cast<UProgressBar>(GetWidgetFromName(MyProgressBar::ProgressBar_Overlay));
		if (UIPtr)
		{
			UIPtr->SetPercent(0);
		}
	}
}
