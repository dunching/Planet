// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "UI/WidgetBase.h"
#include "InteractionBtn.generated.h"

/**
 *
 */
UCLASS()
class SHIYU_API UInteractionBtn : public UWidgetBase
{
	GENERATED_BODY()

public:

	UInteractionBtn(const FObjectInitializer& ObjectInitializer);

	void SetEvent(const std::function<void()>&NewClickEvent);

	void SetTitle(const FString& Title);

protected:

	virtual void NativeConstruct()override;

	UFUNCTION()
		void BtnClick();

private:

	std::function<void()>ClickEvent;

};
