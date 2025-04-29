// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include <AIController.h>

#include "GenerateType.h"
#include "HoverWidgetBase.h"

#include "ItemDecription.generated.h"

struct FBasicProxy;

class UItemProxy_Description;

/**
 * 当鼠标悬浮在ItemProxy上时，显示的说明
 */
UCLASS()
class PLANET_API UItemDecription : public UMyUserWidget
{
	GENERATED_BODY()

public:
	UItemDecription(const FObjectInitializer& ObjectInitializer);

	virtual void BindData(
		const TSharedPtr<FBasicProxy>& ProxySPtr,
		const TSoftObjectPtr<UItemProxy_Description>& ItemProxy_Description
	);

protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);

	TSharedPtr<FBasicProxy> ProxySPtr = nullptr;

	TSoftObjectPtr<UItemProxy_Description> ItemProxy_Description;
private:

	void  UpdatePosstion();
	
	virtual void SetUIStyle();
};
