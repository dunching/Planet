
#include "PlanetAIController.h"

#include <GameFramework/CharacterMovementComponent.h>
#include "Components/StateTreeComponent.h"
#include "Components/StateTreeAIComponent.h"
#include <Perception/AIPerceptionComponent.h>
#include <Kismet/GameplayStatics.h>

#include "CharacterTitle.h"
#include "CharacterBase.h"
#include "AssetRefMap.h"
#include "Planet.h"
#include "GroupMnaggerComponent.h"
#include "SceneElement.h"
#include "HumanCharacter.h"
#include "HoldingItemsComponent.h"
#include "PlanetPlayerController.h"
#include "TestCommand.h"
#include "GameplayTagsSubSystem.h"

UPlanetAbilitySystemComponent* APlanetAIController::GetAbilitySystemComponent() const
{
	return GetPawn<FPawnType>()->GetAbilitySystemComponent();
}

UGroupMnaggerComponent* APlanetAIController::GetGroupMnaggerComponent() const
{
	return GetPawn<FPawnType>()->GetGroupMnaggerComponent();
}

UGourpmateUnit* APlanetAIController::GetGourpMateUnit()
{
	return GourpMateUnitPtr;
}
