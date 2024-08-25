
#include "EQC_Test.h"

#include <EnvironmentQuery/EnvQueryTypes.h>
#include <EnvironmentQuery/Items/EnvQueryItemType_Point.h>
#include <EnvironmentQuery/Items/EnvQueryItemType_Actor.h>
#include <AITypes.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Character.h>
#include "EnvironmentQuery/EQSTestingPawn.h"

#include "HumanCharacter.h"
#include "PlanetPlayerController.h"
#include "HumanAIController.h"

void UEQC_Test::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	UObject* QuerierObject = QueryInstance.Owner.Get();
	if (QuerierObject == nullptr)
	{
		return;
	}

#if WITH_EDITOR
	if (QuerierObject->IsA(AEQSTestingPawn::StaticClass()))
	{
		TArray<AActor*>ResutAry;
		UGameplayStatics::GetAllActorsOfClass(this, AHumanCharacter::StaticClass(), ResutAry);

		if (ResutAry.IsValidIndex(0))
		{
			auto ResultingActor = ResutAry[0];
			UEnvQueryItemType_Actor::SetContextHelper(ContextData, ResultingActor);
		}

		return;
	}
#endif

}

