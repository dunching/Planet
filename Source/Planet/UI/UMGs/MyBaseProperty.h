// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"

#include "MyUserWidget.h"

#include "CharacterAttibutes.h"
#include "GenerateType.h"

#include "MyBaseProperty.generated.h"

struct FOnAttributeChangeData;

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

	void OnValueChanged(const FOnAttributeChangeData& CurrentValue);

	void SetCurrentValue(int32 InCurrentValue);

	void OnValue1Changed(const FOnAttributeChangeData& CurrentValue);

	void OnValue2Changed(const FOnAttributeChangeData& CurrentValue);

	void SetCurrentValue(int32 InValue1, int32 InValue2);

	int32 Value1 = 0;

	int32 Value2 = 0;

};
