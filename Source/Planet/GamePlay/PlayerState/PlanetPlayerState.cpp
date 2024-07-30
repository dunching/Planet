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

FSceneUnitContainer& APlanetPlayerState::GetSceneUnitContainer() 
{
	return SceneUnitContainer;
}

void APlanetPlayerState::BeginPlay()
{
	Super::BeginPlay();

	InitialData();
}

void APlanetPlayerState::InitialData()
{
	SceneUnitContainer.AddUnit_Coin(UGameplayTagsSubSystem::GetInstance()->Unit_Coin_Regular, 0);
	SceneUnitContainer.AddUnit_Coin(UGameplayTagsSubSystem::GetInstance()->Unit_Coin_RafflePermanent, 0);
	SceneUnitContainer.AddUnit_Coin(UGameplayTagsSubSystem::GetInstance()->Unit_Coin_RaffleLimit, 0);
}
