#include "GeneratorBase.h"

#include "Net/UnrealNetwork.h"

#include "LogWriter.h"
#include "GroupManagger.h"

#include "PlanetChildActorComponent.h"

AGeneratorBase::AGeneratorBase(const FObjectInitializer& ObjectInitializer) :
	Super()
{
	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	bAlwaysRelevant = true;

	bDirctSpawnChild = false;
}

void AGeneratorBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, GeneratorGuid, COND_None);
}

void AGeneratorBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		FActorSpawnParameters SpawnParameters;

		SpawnParameters.Owner = this;
		SpawnParameters.CustomPreSpawnInitalization = [](AActor* ActorPtr)
		{
			PRINTINVOKEINFO();
			auto GroupManaggerPtr = Cast<AGroupManagger>(ActorPtr);
			if (GroupManaggerPtr)
			{
				GroupManaggerPtr->GroupID = FGuid::NewGuid();
			}
		};

		GroupManaggerPtr = GetWorld()->SpawnActor<AGroupManagger>(
			AGroupManagger::StaticClass(), SpawnParameters
		);
	}
#endif
}

void AGeneratorBase::BeginPlay()
{
	Super::BeginPlay();
}

void AGeneratorBase::SpawnGeneratorActor()
{
	bDirctSpawnChild = true;

	ForEachComponent(true, [](UActorComponent* Comp)
	{
		auto PlanetChildActorComponentPtr = Cast<UPlanetChildActorComponent>(Comp);
		if (PlanetChildActorComponentPtr)
		{
			PlanetChildActorComponentPtr->CreateChildActor();
		}
	});
}
