// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

#include "MyUserWidget.h"

#include "CharacterAttibutes.h"
#include "GenerateType.h"

#include "MyProgressBar.generated.h"

UCLASS()
class PLANET_API UMyProgressBar : public UMyUserWidget
{
	GENERATED_BODY()

public:

	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	void SetDataSource(
		UAbilitySystemComponent*AbilitySystemComponentPtr,
		FGameplayAttribute Attribute,
		float Value,
		FGameplayAttribute MaxAttribute,
		float MaxValue
		);

	void SetCurrentValue(int32 InCurrentValue);

	void SetCurrentValue_Re(const FOnAttributeChangeData& CurrentValue);

	void SetMaxValue(int32 InMaxValue);

	void SetMaxValue_Re(const FOnAttributeChangeData& CurrentValue);

private:

	void ValueChanged();

	int32 CurrentValue = 0;

	int32 MaxValue = 0;

	FValueChangedDelegateHandle OnPPValueChanged;

	FValueChangedDelegateHandle OnMaxPPValueChanged;

};
