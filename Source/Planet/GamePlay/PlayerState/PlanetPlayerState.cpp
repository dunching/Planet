// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "PlanetPlayerState.h"

#include "AbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "HoldingItemsComponent.h"
#include "GameplayTagsSubSystem.h"
#include "ItemProxyContainer.h"
#include "SceneUnitTable.h"
#include "ItemProxy.h"
#include "TeamConfigureomponent.h"

APlanetPlayerState::APlanetPlayerState(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	TeamConfigureompConentPtr = CreateDefaultSubobject<UTeamConfigureComponent>(UTeamConfigureComponent::ComponentName);
}

void APlanetPlayerState::BeginPlay()
{
	Super::BeginPlay();

	InitialData();
}

void APlanetPlayerState::InitialData()
{
}

UTeamConfigureComponent* APlanetPlayerState::GetTeamConfigureomponent() const
{
	return TeamConfigureompConentPtr;
}
