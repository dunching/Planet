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

	virtual void ResetToolUIByData(
		const FGameplayTag &InProxyType
	);

	protected:
	
	 virtual void NativeOnMouseEnter( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;
	
	 virtual void NativeOnMouseLeave( const FPointerEvent& InMouseEvent ) override;
	
private:
	virtual void SetItemType();

	FGameplayTag ProxyType;

	UPROPERTY(transient)
	UItemDecription * ItemDecriptionPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bIsDisplayInfo = true;
};
