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
	AGroupManagger_NPC* GroupManagger_NPCPtr = nullptr;
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

		GroupManagger_NPCPtr = GetWorld()->SpawnActor<AGroupManagger_NPC>(
		                                                                  AGroupManagger_NPC::StaticClass(),
		                                                                  SpawnParameters
		                                                                 );
		GroupManaggerPtr = GroupManagger_NPCPtr;

		CustomizerGroupManagger(GroupManaggerPtr);

		GroupManaggerPtr->GetTeamMatesHelperComponent()->SwitchTeammateOption(DefaultTeammateOption);
	}

	bool bIsFirst = true;
	int32 Index = 0;
	ForEachComponent(
	                 true,
	                 [&bIsFirst, this, GroupManagger_NPCPtr, &Index](
	                 UActorComponent* Comp
	                 )
	                 {
		                 auto PlanetChildActorComponentPtr = Cast<UPlanetChildActorComponent>(Comp);
		                 if (PlanetChildActorComponentPtr)
		                 {
			                 PlanetChildActorComponentPtr->RespawnChildActor();
			                 auto AICharacterPtr = Cast<AHumanCharacter_AI>(
			                                                                PlanetChildActorComponentPtr->
			                                                                GetChildActor()
			                                                               );
			                 if (!AICharacterPtr)
			                 {
				                 return;
			                 }
			                 AICharacterPtr->GetAIComponent()->bIsSingle = false;

			                 GroupManagger_NPCPtr->GetTeamMatesHelperComponent()->UpdateTeammateConfig(
				                  AICharacterPtr->GetCharacterProxy(),
				                  Index++
				                 );
			                 GroupManagger_NPCPtr->AddSpwanedCharacter(AICharacterPtr);

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
