 #include "GeneratorBase.h"

#include "Net/UnrealNetwork.h"

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
}

void AGeneratorBase::CustomizerFunc(
	AActor* TargetActorPtr
	)
{
}
