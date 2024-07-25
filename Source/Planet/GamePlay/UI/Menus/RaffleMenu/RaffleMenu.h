// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"

#include "GenerateType.h"
#include "RaffleCommon.h"

#include "RaffleMenu.generated.h"

class UTalentIcon;
class URaffleType;
class URaffleBtn;
struct FSceneToolsContainer;

/**
 *
 */
UCLASS()
class PLANET_API URaffleMenu : public UMyUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct()override;

	void InitialRaffleType();

	void InitialRaffleBtn();

	virtual void NativeDestruct()override;

	void SetHoldItemProperty(const FSceneToolsContainer& NewSPHoldItemPerperty);

protected:

	void OnRaffleTypeSelected(URaffleType* RaffleTypePtr);
	
	void OnRaffleBtnSelected(URaffleBtn* RaffleTypePtr);

	URaffleType* PreviouRaffleTypePtr = nullptr;

	ERaffleType LastRaffleType = ERaffleType::kRafflePermanent;

};
