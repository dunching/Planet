
#include "GeneratorColony.h"

#include "Component/PlanetChildActorComponent.h"
#include "BuildingArea.h"
#include "HumanCharacter.h"
#include "HumanAIController.h"

AGeneratorColony::AGeneratorColony(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("USceneComponent"));
}

void AGeneratorColony::BeginPlay()
{
	Super::BeginPlay();

	{
		TArray<UChildActorComponent*>Components;
		GetComponents(UChildActorComponent::StaticClass(), Components);
		for (const auto Iter : Components)
		{
			Iter->CreateChildActor();
		}
	}

	// 
	ABuildingArea* BuildingAreaPtr = nullptr;
	{
		TArray<UChildActorComponent*>Components;
		GetComponents(UChildActorComponent::StaticClass(), Components);
		for (const auto Iter : Components)
		{
			if (Iter->GetChildActor()->IsA(ABuildingArea::StaticClass()))
			{
				BuildingAreaPtr = Cast<ABuildingArea>(Iter->GetChildActor());
			}
		}
	}

	// 
	{
		AHumanCharacter * FirstCharacterPtr = nullptr;

		TArray<UChildActorComponent*>Components;
		GetComponents(UChildActorComponent::StaticClass(), Components);

		for (const auto Iter : Components)
		{
			if (Iter->GetChildActor()->IsA(AHumanCharacter::StaticClass()))
			{
				auto CharacterPtr = Cast<AHumanCharacter>(Iter->GetChildActor());
				if (CharacterPtr)
				{
					if (FirstCharacterPtr)
					{
						FirstCharacterPtr->GetGroupMnaggerComponent()->AddCharacterToGroup(CharacterPtr->GetCharacterUnit());
					}
					else
					{
						FirstCharacterPtr = CharacterPtr;
					}

					auto AIControllerPtr = CharacterPtr->GetController<AHumanAIController>();
					if (AIControllerPtr)
					{
						AIControllerPtr->BuildingArea = BuildingAreaPtr;
					}
				}
			}
		}
	}

	ForEachComponent<UChildActorComponent>(true, [](UChildActorComponent*ComPtr) {
		
		});
}

