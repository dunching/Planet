// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include <AIController.h>

#include "GenerateType.h"
#include "HoverWidgetBase.h"

#include "FocusIcon.generated.h"

class ACharacterBase;

class UToolIcon;

/**
 * 锁定目标时，目标身上的标记
 */
UCLASS()
class PLANET_API UFocusIcon : public UHoverWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime
	);

	virtual FVector GetHoverPosition() override;

	TWeakObjectPtr<ACharacterBase> TargetCharacterPtr = nullptr;

protected:

private:
};
