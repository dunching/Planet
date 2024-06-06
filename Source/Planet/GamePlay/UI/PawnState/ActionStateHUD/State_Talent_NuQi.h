// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "GenerateType.h"

#include "State_Talent_NuQi.generated.h"

UCLASS()
class PLANET_API UState_Talent_NuQi : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	using FDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

protected:

	void OnNuQiCurrentValueChanged(int32 CurrentValue);

	void OnNuQiValueChanged();

	UFUNCTION(BlueprintImplementableEvent)
	void SetNuQiPercent(float Percent);

	FDelegateHandle OnValueChanged;

	int32 NuQiCurrentValue = 0;
	
	int32 NuQiMaxValue = 0;

};
