// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "UIInterfaces.h"

#include "CoinInfo.generated.h"

struct FCoinProxy;

/**
 *
 */
UCLASS()
class PLANET_API UCoinInfo : 
	public UMyUserWidget,
	public IUnitIconInterface
{
	GENERATED_BODY()

public:

	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

protected:

	void SetNum(int32 NewNum);

	void SetItemType();

	TSharedPtr<FCoinProxy> UnitPtr = nullptr;

	FValueChangedDelegateHandle OnNumChanged;

};
