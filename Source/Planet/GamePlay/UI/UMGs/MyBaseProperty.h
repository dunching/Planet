// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "CharacterAttibutes.h"
#include "GenerateType.h"

#include "MyBaseProperty.generated.h"

UCLASS()
class PLANET_API UMyBaseProperty : public UMyUserWidget
{
	GENERATED_BODY()

public:

	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	void SetDataSource(FBasePropertySet& Property);

	void SetDataSource(FBasePropertySet& Property1, FBasePropertySet& Property2);

private:

	void SetCurrentValue(int32 InCurrentValue);

	void SetCurrentValue1(int32 InCurrentValue);

	void SetCurrentValue2(int32 InCurrentValue);

	void ValueChanged();

	int32 Value1 = 0;

	int32 Value2 = 0;

	FValueChangedDelegateHandle OnValueChanged;

	FValueChangedDelegateHandle OnValueChanged2;

};
