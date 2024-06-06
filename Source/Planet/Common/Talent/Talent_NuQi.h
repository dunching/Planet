// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Talent_Base.h"

#include "GenerateType.h"

class FTalent_NuQi : public FTalent_Base
{
public:

	int32 GetCurrentValue() const;

	int32 GetMaxValue() const;

	void SetCurrentValue(int32 val);

	void AddCurrentValue(int32 val);

	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;

private:

	int32 CurrentValue = 0;

	int32 MaxValue = 100;

};
