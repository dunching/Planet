
#include "ResourceBoxBase.h"

#include <Components/SceneComponent.h>

#include "HoldingItemsComponent.h"

AResourceBoxBase::AResourceBoxBase(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));

	HoldingItemsComponentPtr = CreateDefaultSubobject<UHoldingItemsComponent>(UHoldingItemsComponent::ComponentName);
}

void AResourceBoxBase::BeginPlay()
{
	Super::BeginPlay();

	for (const auto Iter : SkillUnitMap)
	{
		HoldingItemsComponentPtr->GetHoldItemProperty().AddUnit(Iter.Key);
	}
	for (const auto Iter : SkillUnitMap)
	{
		HoldingItemsComponentPtr->GetHoldItemProperty().AddUnit(Iter.Key);
	}
}

