
#include "PlanetChildActorComponent.h"

#include "CharacterBase.h"
#include "BuildingBase.h"
#include "BuildingArea.h"
#include "HumanCharacter.h"
#include "HumanAIController.h"

void UPlanetChildActorComponent::CreateChildActor(TFunction<void(AActor*)> CustomizerFunc /*= nullptr*/)
{
#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		Super::CreateChildActor(CustomizerFunc);
	}
#endif
}

