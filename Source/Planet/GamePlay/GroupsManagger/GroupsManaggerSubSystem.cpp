
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
	return Cast<UGroupsManaggerSubSystem>(USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorldImp(), UGroupsManaggerSubSystem::StaticClass()));
}

TSharedPtr<UGroupsManaggerSubSystem::FGroupsHelper> UGroupsManaggerSubSystem::CreateGroup(AHumanCharacter* CharacterPtr)
{
	TSharedPtr<FGroupsHelper> ResultSPtr = MakeShared<FGroupsHelper>();
	for (;;)
	{
		ResultSPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		if (GroupsMap.Find(ResultSPtr))
		{
			continue;
		}
		else
		{
			ResultSPtr->OwnerCharacterPtr = CharacterPtr;
			GroupsMap.Add(ResultSPtr);
			break;
		}
	}
	return ResultSPtr;
}

void UGroupsManaggerSubSystem::FGroupsHelper::AddCharacter(AHumanCharacter* CharacterPtr)
{
	CharactersSet.Add(CharacterPtr);

	CharacterPtr->GetController<IPlanetControllerInterface>()->GetGroupMnaggerComponent()->OnAddToNewGroup(OwnerCharacterPtr);

	GroupsChanged.ExcuteCallback(EGroupMateChangeType::kAdd, CharacterPtr);
}
