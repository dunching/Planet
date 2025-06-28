// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "GenerateTypes.h"
#include "LayoutCommon.h"

#include "LayoutInterfacetion.generated.h"

class UMainHUDLayout;
class UOverlaySlot;
class UUserWidget;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class ULayoutInterfacetion : public UInterface
{
	GENERATED_BODY()
};

class PLANET_API ILayoutInterfacetion
{
	GENERATED_BODY()

	friend UMainHUDLayout;

public:
	using FOnQuit = std::function<void()>;

	virtual void Enable();

	virtual void DisEnable();

	virtual ELayoutCommon GetLayoutType() const = 0;

	/**
	 * 在Layout上显示一些Widget
	 * @param WidgetClass 
	 * @param Initializer 
	 * @return 
	 */
	virtual UOverlaySlot* DisplayWidget(
		const TSubclassOf<UUserWidget>& WidgetClass,
		const std::function<void(UUserWidget*)>& Initializer
		);

	virtual bool RemovedWidgets();
	
	virtual void RemoveWidget(const TSubclassOf<UUserWidget>& WidgetClass);
	
protected:
	bool bIsActive = false;

private:
	FOnQuit OnQuit;
};

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class ULayoutItemInterfacetion : public ULayoutInterfacetion
{
	GENERATED_BODY()
};

class PLANET_API ILayoutItemInterfacetion : public ILayoutInterfacetion
{
	GENERATED_BODY()

private:
	virtual ELayoutCommon GetLayoutType() const override final;
};
