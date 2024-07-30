
#include "HoldingItemsComponent.h"

#include <GameFramework/PlayerState.h>

#include "GameplayTagsSubSystem.h"
#include "CharacterBase.h"
#include "PlanetPlayerState.h"

UHoldingItemsComponent::UHoldingItemsComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
}

FSceneUnitContainer& UHoldingItemsComponent::GetHoldItemProperty()
{
	return HoldItemProperty;
}

void UHoldingItemsComponent::BeginPlay()
{
	Super::BeginPlay();
}

FName UHoldingItemsComponent::ComponentName = TEXT("HoldingItemsComponent");
