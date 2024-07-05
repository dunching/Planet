
#include "GeneratorNPC.h"

#include "Component/PlanetChildActorComponent.h"

AGeneratorNPC::AGeneratorNPC(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("USceneComponent"));
}

void AGeneratorNPC::BeginPlay()
{
	Super::BeginPlay();

	TArray<UChildActorComponent*>Components;
	GetComponents(UChildActorComponent::StaticClass(), Components);
	for (const auto Iter : Components)
	{
// 		FActorSpawnParameters Params;
// 		FVector Location = Iter->GetComponentLocation();
// 		FRotator Rotation = Iter->GetComponentRotation();
// 		auto ActorPtr = GetWorld()->SpawnActor(Iter->GetChildActorClass(), &Location, &Rotation, Params);

//		Iter->DestroyChildActor();
		Iter->CreateChildActor();
	}

	ForEachComponent<UChildActorComponent>(true, [](UChildActorComponent*ComPtr) {
		
		});
}

