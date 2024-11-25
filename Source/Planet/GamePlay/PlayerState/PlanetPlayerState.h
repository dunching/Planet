// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerState.h"

#include "PlanetAbilitySystemComponent.h"
#include "ItemProxy.h"

#include "PlanetPlayerState.generated.h"

class UPlanetAbilitySystemComponent;
class UTeamConfigureComponent;
struct FCharacterProxy;
struct FSceneUnitContainer;

UCLASS()
class PLANET_API APlanetPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	APlanetPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UTeamConfigureComponent* GetTeamConfigureomponent()const;
	
	UPROPERTY()
	TObjectPtr<UTeamConfigureComponent> TeamConfigureompConentPtr = nullptr;
	
protected:

	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	void InitialData();
	
private:

};
