// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "RaffleCommon.h"
#include "UIInterfaces.h"

#include "Raffle_Unit.generated.h"

class UTalentIcon;
struct FSceneUnitExtendInfoBase;

/**
 *
 */
UCLASS()
class PLANET_API URaffle_Unit :
	public UMyUserWidget,
	public IToolsIconInterface
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	virtual void NativeDestruct()override;

	virtual void InvokeReset(UUserWidget* BaseWidgetPtr)override;

	virtual void ResetToolUIByData(UBasicUnit* BasicUnitPtr)override;
	
	virtual void ResetToolUIByData(const FSceneUnitExtendInfoBase& SceneUnitExtendInfoBase);

	virtual void EnableIcon(bool bIsEnable)override;

};
