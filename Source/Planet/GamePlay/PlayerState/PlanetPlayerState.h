// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerState.h"

#include "PlanetAbilitySystemComponent.h"
#include "SceneElement.h"

#include "PlanetPlayerState.generated.h"

class UPlanetAbilitySystemComponent;
class UHoldingItemsComponent;

UCLASS()
class PLANET_API APlanetPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	APlanetPlayerState();

	FSceneUnitContainer& GetSceneUnitContainer();

protected:

	virtual void BeginPlay() override;

	void InitialData();
	
	UPROPERTY(Transient)
	FSceneUnitContainer SceneUnitContainer;

private:

};
