
#include "HoldingItemsComponent.h"

UHoldingItemsComponent::UHoldingItemsComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
}

FSceneToolsContainer& UHoldingItemsComponent::GetHoldItemProperty()
{
	return HoldItemProperty;
}

void UHoldingItemsComponent::BeginPlay()
{
	Super::BeginPlay();
}

FName UHoldingItemsComponent::ComponentName = TEXT("HoldingItemsComponent");
