// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "PlanetPlayerState.h"

#include "AbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "HoldingItemsComponent.h"
#include "GameplayTagsSubSystem.h"

APlanetPlayerState::APlanetPlayerState() :
	Super()
{
	HoldingItemsComponentPtr = CreateDefaultSubobject<UHoldingItemsComponent>(UHoldingItemsComponent::ComponentName);
}

UHoldingItemsComponent* APlanetPlayerState::GetHoldingItemsComponent()
{
	return HoldingItemsComponentPtr;
}

void APlanetPlayerState::BeginPlay()
{
	Super::BeginPlay();

	InitialData();
}

void APlanetPlayerState::InitialData()
{
	auto& HoldItemComponent = HoldingItemsComponentPtr->GetHoldItemProperty();

	HoldItemComponent.AddUnit_Coin(UGameplayTagsSubSystem::GetInstance()->Unit_Coin_Regular, 0);
	HoldItemComponent.AddUnit_Coin(UGameplayTagsSubSystem::GetInstance()->Unit_Coin_RafflePermanent, 0);
	HoldItemComponent.AddUnit_Coin(UGameplayTagsSubSystem::GetInstance()->Unit_Coin_RaffleLimit, 0);
}
