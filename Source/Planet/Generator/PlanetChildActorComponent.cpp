#include "PlanetChildActorComponent.h"

#include "CharacterBase.h"
#include "BuildingBase.h"
#include "BuildingArea.h"
#include "GeneratorBase.h"
#include "HumanCharacter.h"
#include "HumanAIController.h"
#include "HumanCharacter_AI.h"
#include "LogWriter.h"

UPlanetChildActorComponent::UPlanetChildActorComponent(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UPlanetChildActorComponent::CreateChildActor(
	TFunction<void(
		AActor*
	)> CustomizerFunc /*= nullptr*/
)
{
	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
#if UE_EDITOR || UE_SERVER
		if (GetNetMode() == NM_DedicatedServer)
		{
			auto OnwerActorPtr = GetOwner<FOwnerType>();
			if (!OnwerActorPtr)
			{
				return;
			}

			// Kill spawned actor if we have one
			DestroyChildActor();
		}
#endif
	}
	else
	{
		Super::CreateChildActor(CustomizerFunc);
	}
}

void UPlanetChildActorComponent::RespawnChildActor(
	TFunction<void(
		AActor*
	)> CustomizerFunc
)
{
	UWorld* World = GetWorld();
	if ((World->IsGameWorld()))
	{
#if UE_EDITOR || UE_SERVER
		if (GetNetMode() == NM_DedicatedServer)
		{
			auto OnwerActorPtr = GetOwner<FOwnerType>();
			if (!OnwerActorPtr)
			{
				return;
			}

			// Kill spawned actor if we have one
			DestroyChildActor();

			FActorSpawnParameters Params;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			Params.OverrideLevel = (OnwerActorPtr ? OnwerActorPtr->GetLevel() : nullptr);
			Params.CustomPreSpawnInitalization = [OnwerActorPtr](
				AActor* ActorPtr
			)
				{
					PRINTINVOKEINFO();
					auto HumanCharacterPtr = Cast<AHumanCharacter_AI>(ActorPtr);
					if (HumanCharacterPtr)
					{
						HumanCharacterPtr->SetGroupSharedInfo(OnwerActorPtr->GroupManaggerPtr);
					}
				};

			FVector Location = GetComponentLocation();
			FRotator Rotation = GetComponentRotation();
			ChildActor = World->SpawnActor(GetChildActorClass(), &Location, &Rotation, Params);
		}
#endif
	}
}
