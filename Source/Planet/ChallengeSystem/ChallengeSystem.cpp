#include "ChallengeSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "Tools.h"

UChallengeSubSystem* UChallengeSubSystem::GetInstance()
{
	return Cast<UChallengeSubSystem>(
		USubsystemBlueprintLibrary::GetWorldSubsystem(GetWorldImp(), UChallengeSubSystem::StaticClass())
	);
}
