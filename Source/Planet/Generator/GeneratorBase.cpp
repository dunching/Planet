#include "GeneratorBase.h"

#include "AIComponent.h"
#include "CharacterBase.h"
#include "Net/UnrealNetwork.h"

#include "LogWriter.h"
#include "GroupManagger.h"
#include "GroupManagger_NPC.h"
#include "HumanCharacter_AI.h"

#include "PlanetChildActorComponent.h"
#include "TeamMatesHelperComponent.h"

AGeneratorBase::AGeneratorBase(
	const FObjectInitializer& ObjectInitializer
) :
  Super()
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bAlwaysRelevant = true;
}

void AGeneratorBase::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, GeneratorGuid, COND_None);
}

void AGeneratorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AGeneratorBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGeneratorBase::SpawnGeneratorActor()
{
	if (!GroupManaggerPtr)
	{
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.Owner = this;
		SpawnParameters.CustomPreSpawnInitalization = [](
			AActor* ActorPtr
		)
		{
			PRINTINVOKEINFO();
			auto GroupManaggerPtr = Cast<AGroupManagger>(ActorPtr);
			if (GroupManaggerPtr)
			{
				GroupManaggerPtr->GroupID = FGuid::NewGuid();
			}
		};

		GroupManaggerPtr = GetWorld()->SpawnActor<AGroupManagger_NPC>(
			AGroupManagger_NPC::StaticClass(),
			SpawnParameters
		);
		
		CustomizerGroupManagger(GroupManaggerPtr);
		
		GroupManaggerPtr->GetTeamMatesHelperComponent()->SwitchTeammateOption(DefaultTeammateOption);
	}

	bool bIsFirst = true;
	ForEachComponent(
		true,
		[&bIsFirst, this](
		UActorComponent* Comp
	)
		{
			auto PlanetChildActorComponentPtr = Cast<UPlanetChildActorComponent>(Comp);
			if (PlanetChildActorComponentPtr)
			{
				PlanetChildActorComponentPtr->RespawnChildActor();
				auto AICharacterPtr = Cast<AHumanCharacter_AI>(PlanetChildActorComponentPtr->GetChildActor());
				if (!AICharacterPtr)
				{
					return;
				}
				AICharacterPtr->GetAIComponent()->bIsSingle = false;

				if (bIsFirst)
				{
					GroupManaggerPtr->SetOwnerCharacterProxyPtr(
						AICharacterPtr
					);
					bIsFirst = false;
				}
			}
		}
	);
}

void AGeneratorBase::CustomizerFunc(
	AActor* TargetActorPtr
)
{
}

void AGeneratorBase::CustomizerGroupManagger(
	AGroupManagger_NPC* TargetActorPtr
)
{
}
