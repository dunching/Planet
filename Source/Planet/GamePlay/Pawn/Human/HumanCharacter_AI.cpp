
#include "HumanCharacter_AI.h"

#include "GeneratorNPCs_Patrol.h"
#include "AIComponent.h"

AHumanCharacter_NPC::AHumanCharacter_NPC(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	AIComponentPtr = CreateDefaultSubobject<UAIComponent>(TEXT("AIComponent"));
}

void AHumanCharacter_NPC::BeginPlay()
{
	Super::BeginPlay();
}

