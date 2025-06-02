// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "HoverWidgetBase.generated.h"

class ACharacterBase;

class UToolIcon;

/**
 * 悬浮的Widget
 */
UCLASS()
class COMMON_UMG_API UHoverWidgetBase : public UUserWidget_Override
{
	GENERATED_BODY()

public:

	virtual FVector2D ModifyProjectedLocalPosition(
		const FGeometry& ViewportGeometry,
		const FVector2D& LocalPosition
	);

	virtual FVector GetHoverPosition();

	/** The size of the displayed quad. */
	UPROPERTY(EditAnywhere, Category=UserInterface)
	FIntPoint DrawSize = FIntPoint(100, 100);

	/** The Alignment/Pivot point that the widget is placed at relative to the position. */
	UPROPERTY(EditAnywhere, Category=UserInterface)
	FVector2D Pivot = FVector2D(.5f, .5f);

	UPROPERTY(EditAnywhere, Category=UserInterface)
	bool bDrawAtDesiredSize = true;

	bool bShouldCalcOutsideViewPosition = false;
};
