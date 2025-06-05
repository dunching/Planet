
#include "Planet_Consumable_Base.h"

#include "CharacterBase.h"

USceneActorInteractionComponent* APlanet_Consumable_Base::GetSceneActorInteractionComponent() const
{
	return nullptr;
}

void APlanet_Consumable_Base::HasbeenInteracted(
	ACharacterBase* CharacterPtr
	)
{
}

void APlanet_Consumable_Base::HasBeenStartedLookAt(
	ACharacterBase* CharacterPtr
	)
{
}

void APlanet_Consumable_Base::HasBeenLookingAt(
	ACharacterBase* CharacterPtr
	)
{
}

void APlanet_Consumable_Base::HasBeenEndedLookAt()
{
}
