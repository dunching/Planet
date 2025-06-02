#include "EQC_FindTarget.h"

#include <EnvironmentQuery/EnvQueryTypes.h>
#include <EnvironmentQuery/Items/EnvQueryItemType_Point.h>
#include <EnvironmentQuery/Items/EnvQueryItemType_Actor.h>
#include <AITypes.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/Character.h>

#include "GroupManagger.h"
#include "EnvironmentQuery/EQSTestingPawn.h"

#include "HumanCharacter.h"
#include "PlanetPlayerController.h"
#include "HumanAIController.h"
#include "StateProcessorComponent.h"
#include "TeamMatesHelperComponent.h"

void UEQC_Test::ProvideContext(
	FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData
) const
{
	UObject* QuerierObject = QueryInstance.Owner.Get();
	if (QuerierObject == nullptr)
	{
		return;
	}

#if WITH_EDITOR
	if (QuerierObject->IsA(AEQSTestingPawn::StaticClass()))
	{
		TArray<AActor*> ResutAry;
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


void UEQC_GetTarget::ProvideContext(
	FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData
) const
{
	UObject* QuerierObject = QueryInstance.Owner.Get();
	if (QuerierObject == nullptr)
	{
		return;
	}

	auto CharacterPtr = Cast<AHumanCharacter>(QuerierObject);

	if (CharacterPtr)
	{
		auto TargetCharactersAry = CharacterPtr->GetStateProcessorComponent()->GetTargetCharactersAry();
		if (TargetCharactersAry.IsEmpty())
		{}
		else
		{
			TWeakObjectPtr<ACharacterBase> TargetPtr = TargetCharactersAry[0];
			if (TargetPtr.IsValid())
			{
				UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetPtr.Get());
			}
		}
	}
}


void UEQC_GetPlayerFocus::ProvideContext(
	FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData
) const
{
	UObject* QuerierObject = QueryInstance.Owner.Get();
	if (QuerierObject == nullptr)
	{
		return;
	}

	auto CharacterPtr = Cast<AHumanCharacter>(QuerierObject);

	if (CharacterPtr)
	{
		auto PCPtr = CharacterPtr->GetController<AHumanAIController>();
		if (PCPtr)
		{
			auto TargetCharacterPtr = PCPtr->GetTeamFocusTarget();
			if (TargetCharacterPtr.IsValid())
			{
				UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetCharacterPtr.Get());
			}
		}
	}
}

void UEQC_GetLeader::ProvideContext(
	FEnvQueryInstance& QueryInstance,
	FEnvQueryContextData& ContextData
) const
{
	UObject* QuerierObject = QueryInstance.Owner.Get();
	if (QuerierObject == nullptr)
	{
		return;
	}

	auto CharacterPtr = Cast<AHumanCharacter>(QuerierObject);

	if (CharacterPtr)
	{
		auto OwnerCharacterProxyPtr = CharacterPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->
		                                            GetOwnerCharacterProxy();
		if (OwnerCharacterProxyPtr && OwnerCharacterProxyPtr.IsValid())
		{
			auto TargetCharacterPtr = OwnerCharacterProxyPtr->GetCharacterActor();
			if (TargetCharacterPtr.IsValid())
			{
				UEnvQueryItemType_Actor::SetContextHelper(ContextData, TargetCharacterPtr.Get());
			}
		}
	}
}
