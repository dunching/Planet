// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "UIInterfaces.h"

#include "CoinInfo.generated.h"

class UCoinUnit;

/**
 *
 */
UCLASS()
class PLANET_API UCoinInfo : 
	public UMyUserWidget,
	public IToolsIconInterface
{
	GENERATED_BODY()

public:

	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

protected:

	void SetNum(int32 NewNum);

	void SetItemType();

	UCoinUnit* UnitPtr = nullptr;

	FValueChangedDelegateHandle OnNumChanged;

};
