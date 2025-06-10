// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UserWidget_Override.h"

#include "GenerateTypes.h"
#include "MenuInterface.h"
#include "ScaleableWidget.h"

#include "ViewMapMenu.generated.h"

class UBorder;
class UTextBlock;
class UButton;

class ATeleport;

/**
 *
 */
UCLASS()
class PLANET_API UViewMapMenu :
	public UScaleableWidget,
	public IMenuInterface
{
	GENERATED_BODY()

public:

protected:

	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
		) override;

	virtual FReply NativeOnMouseButtonUp(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
		) override;

	virtual void EnableMenu() override;

	virtual void DisEnableMenu() override;

	virtual EMenuType GetMenuType() const override final;

private:
	UFUNCTION()
	void OnTeleportPointWidgetClicked(
		UTeleportPointWidget* TeleportPointWidgetPtr
		);

	UFUNCTION()
	void OnTeleportClicked();

	void UpdatePlayerMarkPt()const;
	
	UPROPERTY(meta = (BindWidget))
	UBorder* TeleportDecriptionBorder = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UButton* TeleportBtn = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TeleportName = nullptr;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* TeleportDescription = nullptr;

	UPROPERTY(meta = (BindWidget))
	UBorder* PlayerBorder = nullptr;

	TSoftObjectPtr<ATeleport>TeleportPtr = nullptr;

	FBoxSphereBounds OpenWorldBoundBox;

	FTimerHandle UpdatePlayerMarkPtHandle;
};
