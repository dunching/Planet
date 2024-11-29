// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "PlanetPlayerState.h"

#include "AbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "HoldingItemsComponent.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Container.h"
#include "SceneUnitTable.h"
#include "ItemProxy.h"
#include "ProxySycHelperComponent.h"

APlanetPlayerState::APlanetPlayerState(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
}

void APlanetPlayerState::BeginPlay()
{
	Super::BeginPlay();

	InitialData();
}

void APlanetPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void APlanetPlayerState::InitialData()
{
}
