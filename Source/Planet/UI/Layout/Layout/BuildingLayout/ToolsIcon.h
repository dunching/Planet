// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include <GameplayTagContainer.h>

#include "UIInterfaces.h"
#include "GenerateTypes.h"

#include "ItemProxy_Minimal.h"

#include "ToolsIcon.generated.h"

struct FBasicProxy;
struct FToolProxy;
struct FConsumableProxy;

struct FStreamableHandle;

UCLASS()
class PLANET_API UToolIcon : public UUserWidget_Override, public IItemProxyIconInterface
{
	GENERATED_BODY()

public:

	using FOnResetProxy = TCallbackHandleContainer<void(const TSharedPtr<FBasicProxy>&)>;

	UToolIcon(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	TSharedPtr<FConsumableProxy> GetConsumablesProxy()const;

	void OnSublingIconReset(const TSharedPtr<FBasicProxy>& InToolProxyPtr);

	FOnResetProxy OnResetProxy;

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

	TSharedPtr<FBasicProxy>ProxyPtr = nullptr;

};
