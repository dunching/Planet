// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "UIInterfaces.h"
#include "GenerateType.h"

#include "ItemProxy_Minimal.h"

#include "ToolsIcon.generated.h"

struct FBasicProxy;
struct FToolProxy;

struct FStreamableHandle;

UCLASS()
class PLANET_API UToolIcon : public UMyUserWidget, public IUnitIconInterface
{
	GENERATED_BODY()

public:

	using FOnResetUnit = TCallbackHandleContainer<void(const TSharedPtr<FBasicProxy>&)>;

	UToolIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicUnitPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	TSharedPtr<FToolProxy> GetToolUnit()const;

	TSharedPtr<FConsumableProxy> GetConsumablesUnit()const;

	void OnSublingIconReset(const TSharedPtr<FBasicProxy>& InToolUnitPtr);

	FOnResetUnit OnResetUnit;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "SkillSocket")
	FGameplayTag IconSocket;

protected:

	void SetNum();

	void SetItemType();

	void ResetSize(const FVector2D& Size);

	virtual void NativeConstruct()override;

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent
	)override;

	virtual bool NativeOnDrop(
		const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation
	)override;

private:

	TSharedPtr<FBasicProxy>UnitPtr = nullptr;

};
