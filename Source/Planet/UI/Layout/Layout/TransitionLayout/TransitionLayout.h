// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "GenerateType.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"


#include "TransitionLayout.generated.h"

class UToolsMenu;



struct FToolsSocketInfo;
struct FConsumableSocketInfo;

/**
 * 濒死状态
 */
UCLASS()
class PLANET_API UTransitionLayout :
	public UMyUserWidget,
	public ILayoutInterfacetion
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void Enable() override;

	virtual void DisEnable() override;

	virtual ELayoutCommon GetLayoutType() const override final;

private:
};
