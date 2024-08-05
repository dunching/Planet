// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "RaffleCommon.h"
#include "UIInterfaces.h"

#include "Raffle_Unit.generated.h"

class UTalentIcon;
struct FTableRowUnit;

/**
 *
 */
UCLASS()
class PLANET_API URaffle_Unit :
	public UMyUserWidget,
	public IUnitIconInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;
	
	virtual void ResetToolUIByData(FTableRowUnit * TableRowUnitPtr);

	virtual void EnableIcon(bool bIsEnable)override;

};
