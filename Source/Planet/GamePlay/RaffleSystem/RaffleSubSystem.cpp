
#include "RaffleSubSystem.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>

#include "Planet_Tools.h"

URaffleSubSystem* URaffleSubSystem::GetInstance()
{
	return Cast<URaffleSubSystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorldImp(), URaffleSubSystem::StaticClass())
	);
}

void URaffleSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

bool URaffleSubSystem::Raffle(ERaffleType RaffleType, int32 Count) const
{


	return false;
}
