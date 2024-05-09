
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

TSharedPtr<UGroupsManaggerSubSystem::FGroupMatesHelper> UGroupsManaggerSubSystem::CreateGroup(IPlanetControllerInterface* PCPtr)
{
	TSharedPtr<FGroupMatesHelper> ResultSPtr = MakeShared<FGroupMatesHelper>();
	for (;;)
	{
		ResultSPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		if (GroupMatesMap.Find(ResultSPtr))
		{
			continue;
		}
		else
		{
			ResultSPtr->OwnerPCPtr = PCPtr;
			GroupMatesMap.Add(ResultSPtr);
			break;
		}
	}
	return ResultSPtr;
}

TSharedPtr<UGroupsManaggerSubSystem::FTeamMatesHelper> UGroupsManaggerSubSystem::CreateTeam(IPlanetControllerInterface* PCPtr)
{
	TSharedPtr<FTeamMatesHelper> ResultSPtr = MakeShared<FTeamMatesHelper>();
	for (;;)
	{
		ResultSPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		if (TeamMatesMap.Find(ResultSPtr))
		{
			continue;
		}
		else
		{
			ResultSPtr->OwnerPCPtr = PCPtr;
			TeamMatesMap.Add(ResultSPtr);
			break;
		}
	}
	return ResultSPtr;
}

void UGroupsManaggerSubSystem::FGroupMatesHelper::AddCharacter(IPlanetControllerInterface* PCPtr)
{
	MembersSet.Add(PCPtr);

	PCPtr->GetGroupMnaggerComponent()->OnAddToNewGroup(OwnerPCPtr);

	MembersChanged.ExcuteCallback(EGroupMateChangeType::kAdd, PCPtr);
}

void UGroupsManaggerSubSystem::FTeamMatesHelper::AddCharacter(const FGameplayTag& Tag, IPlanetControllerInterface* PCPtr)
{
	MembersMap.Add(Tag, PCPtr);

	PCPtr->GetGroupMnaggerComponent()->OnAddToNewGroup(OwnerPCPtr);

	MembersChanged.ExcuteCallback(EGroupMateChangeType::kAdd, PCPtr);
}
