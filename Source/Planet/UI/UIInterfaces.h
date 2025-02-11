// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"

#include "GenerateType.h"
#include "ItemProxy_Minimal.h"

#include "UIInterfaces.generated.h"

struct FBasicProxy;
struct FAllocationbleProxy;

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UItemProxyIconInterface : public UInterface
{
	GENERATED_BODY()
};

class PLANET_API IItemProxyIconInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	virtual void InvokeReset(UUserWidget* BaseWidgetPtr) = 0;
	
	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr) = 0;

	virtual void EnableIcon(bool bIsEnable) = 0;

protected:

private:

};

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UAllocationableProxyIconInterface : public UInterface
{
	GENERATED_BODY()
};

class PLANET_API IAllocationableProxyIconInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	virtual void InvokeReset(UUserWidget* BaseWidgetPtr) = 0;
	
	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr) = 0;

	virtual void EnableIcon(bool bIsEnable) = 0;

protected:

private:

};
