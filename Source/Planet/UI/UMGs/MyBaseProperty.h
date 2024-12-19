// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttibutes.h"
#include "GenerateType.h"

#include "MyBaseProperty.generated.h"

class UAbilitySystemComponent;

UCLASS()
class PLANET_API UMyBaseProperty : public UMyUserWidget
{
	GENERATED_BODY()

public:

	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	void SetDataSource(
		UAbilitySystemComponent*AbilitySystemComponentPtr,
		FGameplayAttribute Attribute,
		float Value
		);

	void SetDataSource(
		UAbilitySystemComponent*AbilitySystemComponentPtr,
		FGameplayAttribute Attribute,
		float Value,
		FGameplayAttribute MaxAttribute,
		float MaxValue
		);

private:

	void SetCurrentValue_Re(const FOnAttributeChangeData& CurrentValue);

	void SetCurrentValue(int32 InCurrentValue);

	void SetCurrentValue1_Re(const FOnAttributeChangeData& CurrentValue);

	void SetCurrentValue1(int32 InCurrentValue);

	void SetCurrentValue2_Re(const FOnAttributeChangeData& CurrentValue);

	void SetCurrentValue2(int32 InCurrentValue);

	void ValueChanged();

	int32 Value1 = 0;

	int32 Value2 = 0;

	FValueChangedDelegateHandle OnValueChanged;

	FValueChangedDelegateHandle OnValueChanged2;

};
