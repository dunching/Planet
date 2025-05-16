#include "BasePropertyWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "AbilitySystemComponent.h"

namespace MyBaseProperty
{
	const FName ProgressBar = TEXT("ProgressBar");

	const FName Text = TEXT("Text");
}

void UBasePropertyWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UBasePropertyWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

void UBasePropertyWidget::SetDataSource(
	UAbilitySystemComponent* AbilitySystemComponentPtr,
	FGameplayAttribute Attribute,
	float InValue
	)
{
	AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
	                                                                   Attribute
	                                                                  ).AddUObject(this, &ThisClass::OnValueChanged);
	Value = InValue;

	UpdateText();
}

void UBasePropertyWidget::OnValueChanged(
	const FOnAttributeChangeData& CurrentValue
	)
{
	Value = CurrentValue.NewValue;

	UpdateText();
}

void UBasePropertyWidget::UpdateText(
	)
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(MyBaseProperty::Text));
	if (UIPtr)
	{
		UIPtr->SetText(FText::FromString(*FString::Printf(TEXT("%d"), Value)));
	}
}

void UBaseProperty_WithPercent::SetDataSource(
	UAbilitySystemComponent* AbilitySystemComponentPtr,
	FGameplayAttribute Attribute,
	float InValue,
	FGameplayAttribute PercentValueAttribute,
	float InPercentValue
	)
{
	AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
	                                                                   Attribute
	                                                                  ).AddUObject(
		 this,
		 &ThisClass::OnPercentValueChanged
		);

	PercentValue = InPercentValue;

	Super::SetDataSource(AbilitySystemComponentPtr, Attribute, InValue);
}

void UBaseProperty_WithPercent::OnPercentValueChanged(
	const FOnAttributeChangeData& CurrentValue
	)
{
	PercentValue = CurrentValue.NewValue;

	UpdateText();
}

inline void UBaseProperty_WithPercent::UpdateText()
{
}

void UBaseProperty_Elemental::SetDataSource(
	UAbilitySystemComponent* AbilitySystemComponentPtr,
	FGameplayAttribute ValueAttribute,
	float InValue,
	FGameplayAttribute LevelAttribute,
	float InLevel,
	FGameplayAttribute PenetrationAttribute,
	float InPenetration,
	FGameplayAttribute PercentPenetrationAttribute,
	float InPercentPenetration,
	FGameplayAttribute ResistanceAttribute,
	float InResistance
	)
{
	AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
	                                                                   LevelAttribute
	                                                                  ).AddUObject(
		 this,
		 &ThisClass::OnLevelChanged
		);

	Level = InLevel;

	AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
	                                                                   PenetrationAttribute
	                                                                  ).AddUObject(
		 this,
		 &ThisClass::OnPenetrationChanged
		);

	Penetration = InPenetration;

	AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
	                                                                   PercentPenetrationAttribute
	                                                                  ).AddUObject(
		 this,
		 &ThisClass::OnPercentPenetrationChanged
		);

	PercentPenetration = InPercentPenetration;

	AbilitySystemComponentPtr->GetGameplayAttributeValueChangeDelegate(
	                                                                   ResistanceAttribute
	                                                                  ).AddUObject(
		 this,
		 &ThisClass::OnResistanceChanged
		);

	Resistance = InResistance;

	Super::SetDataSource(AbilitySystemComponentPtr, ValueAttribute, InValue);
}

void UBaseProperty_Elemental::OnLevelChanged(
	const FOnAttributeChangeData& CurrentValue
	)
{
	Level = CurrentValue.NewValue;

	UpdateText();
}

void UBaseProperty_Elemental::OnPenetrationChanged(
	const FOnAttributeChangeData& CurrentValue
	)
{
	Penetration = CurrentValue.NewValue;

	UpdateText();
}

void UBaseProperty_Elemental::OnPercentPenetrationChanged(
	const FOnAttributeChangeData& CurrentValue
	)
{
	PercentPenetration = CurrentValue.NewValue;

	UpdateText();
}

void UBaseProperty_Elemental::OnResistanceChanged(
	const FOnAttributeChangeData& CurrentValue
	)
{
	Resistance = CurrentValue.NewValue;

	UpdateText();
}

void UBaseProperty_Elemental::UpdateText()
{
	auto UIPtr = Cast<UTextBlock>(GetWidgetFromName(MyBaseProperty::Text));
	if (UIPtr)
	{
		UIPtr->SetText(
		               FText::FromString(
		                                 *FString::Printf(
		                                                  TEXT("%d/%d/%d/%d/%/%d"),
		                                                  Level,
		                                                  Value,
		                                                  Penetration,
		                                                  PercentPenetration,
		                                                  Resistance
		                                                 )
		                                )
		              );
	}
}
