// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"

#include "MyUserWidget.h"

#include "CharacterAttibutes.h"
#include "GenerateType.h"

#include "BasePropertyWidget.generated.h"

struct FOnAttributeChangeData;

class UAbilitySystemComponent;

/**
 * 单一数据
 */
UCLASS()
class PLANET_API UBasePropertyWidget : public UMyUserWidget
{
	GENERATED_BODY()

public:

	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	void SetDataSource(
		UAbilitySystemComponent*AbilitySystemComponentPtr,
		FGameplayAttribute Attribute,
		float InValue
		);

protected:
	
	void OnValueChanged(const FOnAttributeChangeData& CurrentValue);

	int32 Value = 0;

private:

	virtual void UpdateText();

};

/**
 * 单一数据附带比分比
 */
UCLASS()
class PLANET_API UBaseProperty_WithPercent : public UBasePropertyWidget
{
	GENERATED_BODY()

public:

	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	void SetDataSource(
		UAbilitySystemComponent*AbilitySystemComponentPtr,
		FGameplayAttribute Attribute,
		float InValue,
		FGameplayAttribute PercentValueAttribute,
		float PercentValue
		);

private:

	void OnPercentValueChanged(const FOnAttributeChangeData& CurrentValue);

	virtual void UpdateText() override;

	int32 PercentValue = 0;

};

/**
 * 五行元素
 */
UCLASS()
class PLANET_API UBaseProperty_Elemental : public UBasePropertyWidget
{
	GENERATED_BODY()

public:

	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	void SetDataSource(
		UAbilitySystemComponent*AbilitySystemComponentPtr,
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
		);

private:

	void OnLevelChanged(const FOnAttributeChangeData& CurrentValue);

	void OnPenetrationChanged(const FOnAttributeChangeData& CurrentValue);

	void OnPercentPenetrationChanged(const FOnAttributeChangeData& CurrentValue);

	void OnResistanceChanged(const FOnAttributeChangeData& CurrentValue);

	virtual void UpdateText() override;

	int32 Level = 0;

	int32 Penetration = 0;

	int32 PercentPenetration = 0;

	int32 Resistance = 0;

};

