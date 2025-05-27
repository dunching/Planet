#include "ChallengeSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "WorldPartition/DataLayer/DataLayerManager.h"
#include "Engine/TargetPoint.h"

#include "LogWriter.h"

#include "Planet_Tools.h"
#include "SceneProxyExtendInfo.h"
#include "PlanetPlayerController.h"
#include "Tools.h"

UChallengeSubSystem* UChallengeSubSystem::GetInstance()
{
	return Cast<UChallengeSubSystem>(
		USubsystemBlueprintLibrary::GetWorldSubsystem(GetWorldImp(), UChallengeSubSystem::StaticClass())
	);
}
