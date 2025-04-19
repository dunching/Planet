#include "TransitionLayout.h"

#include "CharacterAbilitySystemComponent.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

#include "GameplayCommand.h"
#include "HumanCharacter_Player.h"
#include "OpenWorldSystem.h"
#include "PlanetPlayerController.h"

struct FTransitionLayout : public TStructVariable<FTransitionLayout>
{
	FName RespawnBtn = TEXT("RespawnBtn");
};

void UTransitionLayout::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTransitionLayout::Enable()
{
}

void UTransitionLayout::DisEnable()
{
}