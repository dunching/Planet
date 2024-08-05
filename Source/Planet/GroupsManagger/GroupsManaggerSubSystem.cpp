
#include "GroupsManaggerSubSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "Planet.h"
#include "GroupMnaggerComponent.h"
#include "CharacterBase.h"
#include "SceneElement.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"

UGroupsManaggerSubSystem* UGroupsManaggerSubSystem::GetInstance()
{
	return Cast<UGroupsManaggerSubSystem>(
		USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorldImp(), UGroupsManaggerSubSystem::StaticClass())
	);
}
