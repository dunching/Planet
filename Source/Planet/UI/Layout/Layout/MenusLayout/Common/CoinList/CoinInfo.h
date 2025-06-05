// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget_Override.h"

#include "ItemProxy_GenericType.h"
#include "LayoutInterfacetion.h"
#include "UIInterfaces.h"

#include "CoinInfo.generated.h"

struct FCoinProxy;
struct FCoinProxy;

/**
 *
 */
UCLASS()
class PLANET_API UCoinInfo :
	public UUserWidget_Override,
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:
	using FValueChangedDelegateHandle = TOnValueChangedCallbackContainer<int32>::FCallbackHandleSPtr;

	virtual void NativeConstruct() override;

	virtual void Enable() override;

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
