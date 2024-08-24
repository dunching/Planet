
#include "EQC_FindEnemy.h"

#include <EnvironmentQuery/EnvQueryTypes.h>
#include <EnvironmentQuery/Items/EnvQueryItemType_Point.h>
#include <EnvironmentQuery/Items/EnvQueryItemType_Actor.h>
#include "EnvironmentQuery/EQSTestingPawn.h"
#include <AITypes.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Character.h>

#include "HumanCharacter.h"
#include "PlanetPlayerController.h"
#include "HumanAIController.h"

void UEQC_FindEnemy::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	UObject* QuerierObject = QueryInstance.Owner.Get();
	if (QuerierObject == nullptr)
	{
		return;
	}

	auto CharacterPtr = Cast<AHumanCharacter>(QuerierObject);

	if (CharacterPtr)
	{
		auto ControllerPtr = CharacterPtr->GetController<AHumanAIController>();
		if (ControllerPtr)
		{
			auto TargetCharacterPtr = ControllerPtr->GetTeamFocusTarget();
			if (TargetCharacterPtr.IsValid())
			{
				UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetCharacterPtr.Get());
			}
		}
	}
}

