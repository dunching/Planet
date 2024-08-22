// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerState.h"

#include "PlanetAbilitySystemComponent.h"
#include "SceneElement.h"

#include "PlanetPlayerState.generated.h"

class UPlanetAbilitySystemComponent;
class UHoldingItemsComponent;
class UCharacterUnit;

UCLASS()
class PLANET_API APlanetPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	APlanetPlayerState();
	
protected:

	virtual void BeginPlay() override;

	void InitialData();

private:

};
