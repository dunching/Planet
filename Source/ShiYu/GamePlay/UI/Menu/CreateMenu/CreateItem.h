// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetBase.h"

#include "Common/GenerateType.h"
#include "Common/ItemType.h"
#include "CreateQueue/CreateQueueMG.h"

#include "CreateItem.generated.h"

class APlayerCharacter;
class UMainUI;

/**
 *
 */
UCLASS()
class SHIYU_API UCreateItem : public UWidgetBase
{
	GENERATED_BODY()

private:

	using FOnCreateItem = std::function<void(FItemNum)>;

public:

	UCreateItem(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct()override;

	UFUNCTION(BlueprintCallable)
		void ResetWidget(UCreateItem* TargetUIPtr);

	void IsAbleCreateItem(bool bIsAbleCreate);

	void SetOnCreateItem(const FOnCreateItem& NewOnCreateItem);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Val)
		FItemNum ItemsType;

protected:

	UFUNCTION(BlueprintCallable)
		void OnCreateBtnClick();

private:

	FOnCreateItem OnCreateItem;

};
