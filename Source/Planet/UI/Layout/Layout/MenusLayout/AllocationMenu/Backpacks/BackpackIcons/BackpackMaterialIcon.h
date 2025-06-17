// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BackpackIcon.h"

#include "BackpackMaterialIcon.generated.h"

struct FStreamableHandle;

struct FBasicProxy;
struct FSkillProxy;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackMaterialIcon :
	public UBackpackIcon
{
	GENERATED_BODY()

public:
	using FProxyType = FMaterialProxy;
	
	UBackpackMaterialIcon(
		const FObjectInitializer& ObjectInitializer
		);

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual void InvokeReset(
		UUserWidget* BaseWidgetPtr
		) override;

	virtual void ResetToolUIByData(
		const TSharedPtr<FBasicProxy>& BasicProxyPtr
		) override;

	virtual void OnAllocationCharacterProxyChanged(
		const TWeakPtr<FCharacterProxy>& AllocationCharacterProxyPtr
		)override final;

	virtual void EnableIcon(
		bool bIsEnable
		) override;

protected:
	void SetNum(
		int32 NewNum
		);

	void SetValue(
		int32 Value
		);

	void ResetSize(
		const FVector2D& Size
		);

	virtual void NativeConstruct() override;

private:
	TSharedPtr<FProxyType> ProxyPtr = nullptr;
};
