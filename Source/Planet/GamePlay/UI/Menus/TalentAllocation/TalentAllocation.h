// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "GenerateType.h"

#include "TalentAllocation.generated.h"

/**
 *
 */
UCLASS()
class PLANET_API UTalentAllocation : public UUserWidget
{
	GENERATED_BODY()

public:

	using FDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

protected:

	void OnUsedTalentNumChanged(int32 OldNum, int32 NewNum);

	FDelegateHandle OnValueChanged;

};
