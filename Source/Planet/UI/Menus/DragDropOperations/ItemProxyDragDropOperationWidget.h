// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include "UIInterfaces.h"

#include "ItemProxyDragDropOperationWidget.generated.h"

class UUserWidget;

struct FBasicProxy;
struct FToolProxy;
struct FSkillProxy;
struct FConsumableProxy;

struct FStreamableHandle;

/**
 *
 */
UCLASS()
class PLANET_API UItemProxyDragDropOperationWidget :
	public UMyUserWidget,
	public IItemProxyIconInterface
{
	GENERATED_BODY()

public:

	UItemProxyDragDropOperationWidget(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void SetNum(int32 NewNum);

	void SetItemType(FBasicProxy* ToolSPtr);
	
	void ResetSize(const FVector2D& Size);

protected:

private:

	TSharedPtr<FBasicProxy> BasicProxyPtr = nullptr;

};

UCLASS()
class PLANET_API UAllocationableProxyDragDropOperationWidget :
	public UMyUserWidget,
	public IAllocationableProxyIconInterface
{
	GENERATED_BODY()

public:

	UAllocationableProxyDragDropOperationWidget(const FObjectInitializer& ObjectInitializer);

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FAllocationbleProxy>& BasicProxyPtr)override;

	virtual void EnableIcon(bool bIsEnable)override;

	void SetNum(int32 NewNum);

	void SetItemType(FBasicProxy* ToolSPtr);
	
	void ResetSize(const FVector2D& Size);

protected:

private:

	TSharedPtr<FAllocationbleProxy> BasicProxyPtr = nullptr;

};
