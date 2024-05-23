
#include "EQC_FindTeammate.h"

#include <EnvironmentQuery/EnvQueryTypes.h>
#include <EnvironmentQuery/Items/EnvQueryItemType_Point.h>
#include <EnvironmentQuery/Items/EnvQueryItemType_Actor.h>
#include <AITypes.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Character.h>

void UEQC_FindTeammate::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	UObject* QuerierObject = QueryInstance.Owner.Get();
	if (QuerierObject == nullptr)
	{
		return;
	}

	// NOTE: QuerierActor is redundant with QuerierObject and should be removed in the future.  It's here for now for
	// backwards compatibility.
	AActor* QuerierActor = Cast<AActor>(QuerierObject);

	AActor* ResultingActor = nullptr;
	
	ResultingActor = UGameplayStatics::GetPlayerCharacter(this, 0);

	if (ResultingActor)
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, ResultingActor);
	}
}

