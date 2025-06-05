
#include "Tool_Base.h"

#include "Components/SkeletalMeshComponent.h"

void ATool_Base::AttachToCharacter(ACharacter* CharacterPtr)
{
}

void ATool_Base::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::Destroyed)
	{

	}

	Super::EndPlay(EndPlayReason);
}
