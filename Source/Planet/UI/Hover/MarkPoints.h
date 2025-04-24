// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include <AIController.h>

#include "GenerateType.h"
#include "HoverWidgetBase.h"

#include "MarkPoints.generated.h"

class ACharacterBase;

class UToolIcon;
class ATargetPoint_Runtime;

/**
 * 锁定目标时，目标身上的标记
 */
UCLASS()
class PLANET_API UMarkPoints : public UHoverWidgetBase
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FVector2D ModifyProjectedLocalPosition(
		const FGeometry& ViewportGeometry,
		const FVector2D& LocalPosition
	) override;

	virtual FVector GetHoverPosition() override;

	TObjectPtr<ATargetPoint_Runtime>TargetPoint_RuntimePtr = nullptr;

protected:

private:

};
