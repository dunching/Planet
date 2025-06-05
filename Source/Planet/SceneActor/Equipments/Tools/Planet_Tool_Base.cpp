
#include "Planet_Tool_Base.h"

#include "Components/SkeletalMeshComponent.h"

void APlanet_Tool_Base::AttachToCharacter(ACharacter* CharacterPtr)
{
}

void APlanet_Tool_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{

	}

	Super::EndPlay(EndPlayReason);
}
