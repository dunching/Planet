// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "PlanetPlayerState.h"

#include "AbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "HoldingItemsComponent.h"
#include "GameplayTagsSubSystem.h"

APlanetPlayerState::APlanetPlayerState() :
	Super()
{
}

void APlanetPlayerState::BeginPlay()
{
	Super::BeginPlay();

	InitialData();
}

void APlanetPlayerState::InitialData()
{
}
