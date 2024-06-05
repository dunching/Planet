
#include "EQC_FindEnemy.h"

#include <EnvironmentQuery/EnvQueryTypes.h>
#include <EnvironmentQuery/Items/EnvQueryItemType_Point.h>
#include <EnvironmentQuery/Items/EnvQueryItemType_Actor.h>
#include <AITypes.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Character.h>

#include "HumanCharacter.h"
#include "HumanPlayerController.h"
#include "HumanAIController.h"

void UEQC_FindEnemy::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	UObject* QuerierObject = QueryInstance.Owner.Get();
	if (QuerierObject == nullptr)
	{
		return;
	}

#if WITH_EDITOR
	if (GEditor->IsPlayingSessionInEditor())
	{
	}
	else
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
	auto CharacterPtr = Cast<AHumanCharacter>(QuerierObject);

	if (CharacterPtr)
	{
		auto PCPtr = CharacterPtr->GetController<AHumanAIController>();
		if (PCPtr)
		{
			UEnvQueryItemType_Actor::SetContextHelper(ContextData, PCPtr->GetTeamFocusTarget());
		}
	}
}

