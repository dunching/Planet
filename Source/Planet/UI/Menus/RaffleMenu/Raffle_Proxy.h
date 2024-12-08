// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "RaffleCommon.h"
#include "UIInterfaces.h"

#include "Raffle_Proxy.generated.h"

class UTalentIcon;
struct FTableRowProxy;

/**
 *
 */
UCLASS()
class PLANET_API URaffle_Proxy :
	public UMyUserWidget,
	public IItemProxyIconInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(const TSharedPtr<FBasicProxy>& BasicProxyPtr)override;
	
	virtual void ResetToolUIByData(FTableRowProxy * TableRowProxyPtr);

	virtual void EnableIcon(bool bIsEnable)override;

};

UCLASS()
class PLANET_API URaffle_Unit :
	public URaffle_Proxy
{
	GENERATED_BODY()

};
