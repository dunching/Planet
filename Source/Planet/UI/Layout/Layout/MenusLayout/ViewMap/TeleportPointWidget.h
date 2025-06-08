// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MarkPointWidget.h"

#include "TeleportPointWidget.generated.h"

class UBorder;
class UImage;

class ATeleport;

/**
 *
 */
UCLASS()
class PLANET_API UTeleportPointWidget :
	public UMarkPointWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	virtual void NativeDestruct() override;
	
	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
		) override;

	virtual FReply NativeOnMouseButtonUp(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
		) override;

	TDelegate<void(
		UTeleportPointWidget*
		)> OnClicked;

	UPROPERTY(meta = (BindWidget))
	UBorder* Border = nullptr;

	UPROPERTY(meta = (BindWidget))
	UImage* Image = nullptr;

	UPROPERTY(BlueprintreadWrite, EditAnywhere)
	FColor NormalColor = FColor::White;

	UPROPERTY(BlueprintreadWrite, EditAnywhere)
	FColor SelectedColor = FColor::Green;

	UPROPERTY(BlueprintreadWrite, EditAnywhere)
	TSoftObjectPtr<ATeleport>TeleportRef = nullptr;

private:
};
