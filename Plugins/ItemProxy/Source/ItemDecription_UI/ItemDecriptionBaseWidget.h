// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"
#include <AIController.h>
#include "GameplayTagContainer.h"

#include "ItemDecriptionBaseWidget.generated.h"

struct FBasicProxy;

class UItemProxy_Description;

/**
 * 当鼠标悬浮在ItemProxy上时，显示的说明
 */
UCLASS()
class ITEMPROXY_API UItemDecriptionBaseWidget : public UUserWidget_Override
{
	GENERATED_BODY()

public:
	UItemDecriptionBaseWidget(
		const FObjectInitializer& ObjectInitializer
		);

	virtual void BindData(
		const TSharedPtr<FBasicProxy>& ProxySPtr,
		const TSoftObjectPtr<UItemProxy_Description>& ItemProxy_Description
		);

	virtual void BindData(
		const FGameplayTag &InProxyType,
		const TSoftObjectPtr<UItemProxy_Description>& ItemProxy_Description
	);

protected:
	virtual void NativeConstruct() override;

	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime
		) override;

	/**
	 * 这个里面的数据是动态的，比如我们需要得到一些特殊的加成
	 */
	TSharedPtr<FBasicProxy> ProxySPtr = nullptr;

	FGameplayTag ProxyType;

	TSoftObjectPtr<UItemProxy_Description> ItemProxy_Description;

private:
	void UpdatePosstion();

	virtual void SetUIStyle();
};
