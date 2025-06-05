#include "TransitionSubSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "Planet_Tools.h"
#include "Tools.h"

UTransitionSubSystem* UTransitionSubSystem::GetInstance()
{
	return Cast<UTransitionSubSystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorldImp(), UTransitionSubSystem::StaticClass())
	);
}
