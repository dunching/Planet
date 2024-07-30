// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BackpackIcon.h"

#include "BackpackSkillIcon.generated.h"

struct FStreamableHandle;

class UBasicUnit;
class USkillUnit;

/**
 *
 */
UCLASS()
class PLANET_API UBackpackSkillIcon :
	public UBackpackIcon
{
	GENERATED_BODY()

public:

	using FOnDragDelegate = TCallbackHandleContainer<void(bool, USkillUnit*)>;

	UBackpackSkillIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	FOnDragDelegate OnDragDelegate;

protected:

	void SetNum(int32 NewNum);

	void SetItemType();

	void SetValue(int32 Value);

	void ResetSize(const FVector2D& Size);

	virtual void NativeConstruct()override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

	virtual void NativeOnDragDetected(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation
	)override;
	
	virtual void OnDroped(UDragDropOperation* Operation)override;

private:

	USkillUnit* UnitPtr = nullptr;

};
