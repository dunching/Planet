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
	public UMyUserWidget
{
	GENERATED_BODY()

public:
	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct() override;

	void SetNum(
		int32 NewNum
	);

	void OnCoinProxyChanged(
		const TSharedPtr<
			FCoinProxy>& CoinProxySPtr,
		EProxyModifyType ProxyModifyType,
		int32 Num
	);

	TSharedPtr<FCoinProxy> ProxyPtr = nullptr;

	FValueChangedDelegateHandle OnNumChanged;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Coin")
	FGameplayTag CoinType;
};
