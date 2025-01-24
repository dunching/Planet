// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "PlanetPlayerState.h"

#include "AbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "InventoryComponent.h"
#include "GameplayTagsLibrary.h"
#include "ItemProxy_Container.h"
#include "SceneProxyTable.h"
#include "ItemProxy_Minimal.h"

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
