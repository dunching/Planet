// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "MenuInterface.h"

#include "ProxyIcon.generated.h"

class UTaskItem;
class UTaskItemCategory;
class UItemDecription;

/**
 *
 */
UCLASS()
class PLANET_API UProxyIcon :
	public UMyUserWidget
{
	GENERATED_BODY()

public:

	virtual void ResetToolUIByData(
		const TSharedPtr<FBasicProxy>& BasicProxyPtr
	);

	protected:
	
	 virtual void NativeOnMouseEnter( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;
	
	 virtual void NativeOnMouseLeave( const FPointerEvent& InMouseEvent ) override;
	
private:
	virtual void SetItemType();

	TSharedPtr<FBasicProxy> BasicProxyPtr = nullptr;

	UPROPERTY(transient)
	UItemDecription * ItemDecriptionPtr = nullptr;
};
