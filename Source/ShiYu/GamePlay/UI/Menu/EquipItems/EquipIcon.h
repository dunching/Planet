// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"
#include "Common/GenerateType.h"

#include "ItemType.h"

#include "EquipIcon.generated.h"

class APlayerCharacter;
class UMainUI;

/**
 *
 */
UCLASS()
class SHIYU_API UEquipIcon : public UWidgetBase
{
	GENERATED_BODY()

public:

	using FOnItemChangeCB = std::function<void(FItemNum, EItemChangeType)>;

	UEquipIcon(const FObjectInitializer& ObjectInitializer);

		UFUNCTION(BlueprintCallable)
	void ResetUIByData(const FItemNum& NewItemBase);

	void SetNum(int32 NewNum);

	void SetItemType(FItemNum NewItemsType);

	FItemNum GetItemsType()const;

	int32 GetItemsNum()const;

	const FItemNum& GetItemPropertyBase()const;

protected:

	void ResetSize(const FVector2D& Size);

	virtual void NativeConstruct()override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Val)
		FItemNum ItemPropertyBase;

private:

};
