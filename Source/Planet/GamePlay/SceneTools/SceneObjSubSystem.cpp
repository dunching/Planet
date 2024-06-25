
#include "SceneObjSubSystem.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>

#include "Planet.h"

USceneObjSubSystem* USceneObjSubSystem::GetInstance()
{
	return Cast<USceneObjSubSystem>(USubsystemBlueprintLibrary::GetGameInstanceSubsystem(
		GetWorldImp(), USceneObjSubSystem::StaticClass())
	);
}

APostProcessVolume* USceneObjSubSystem::GetSkillPost() const
{
	APostProcessVolume * ResultPtr = nullptr;

	

	return ResultPtr;
}

