// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"

#include "ItemInteractionItem.generated.h"

class UTextBlock;

UCLASS()
class PLANET_API UItemInteractionItem :
	public UUserWidget_Override,
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:
	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
		) override;

	void SetData(
		const TSharedPtr<FBasicProxy>& ProxySPtr,
		EItemProxyInteractionType ItemProxyInteractionType
		);

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextBlock = nullptr;

	TSharedPtr<FBasicProxy> ProxySPtr = nullptr;

	EItemProxyInteractionType ItemProxyInteractionType = EItemProxyInteractionType::kDiscard;
};
