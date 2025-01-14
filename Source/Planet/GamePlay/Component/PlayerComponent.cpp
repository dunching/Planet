
#include "PlayerComponent.h"

#include "GeneratorColony.h"
#include "CharacterBase.h"
#include "HumanAIController.h"

FName UPlayerComponent::ComponentName = TEXT("PlayerComponent");

UPlayerComponent::UPlayerComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
}

void UPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

}