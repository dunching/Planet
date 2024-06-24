
#include "GameplayTagsSubSystem.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>

#include "Planet.h"

UGameplayTagsSubSystem* UGameplayTagsSubSystem::GetInstance()
{
	return Cast<UGameplayTagsSubSystem>(USubsystemBlueprintLibrary::GetGameInstanceSubsystem(
		GetWorldImp(), UGameplayTagsSubSystem::StaticClass())
	);
}

