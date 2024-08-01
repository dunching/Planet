
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

TSharedPtr<FGroupMatesHelper> UGroupsManaggerSubSystem::CreateGroup(FPawnType* PCPtr)
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
			ResultSPtr->OwnerPtr = PCPtr;
			GroupMatesMap.Add(ResultSPtr);
			break;
		}
	}
	return ResultSPtr;
}

TSharedPtr<FTeamMatesHelper> UGroupsManaggerSubSystem::CreateTeam(FPawnType* PCPtr)
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
			ResultSPtr->OwnerPtr = PCPtr;
			TeamMatesMap.Add(ResultSPtr);
			break;
		}
	}
	return ResultSPtr;
}

void FGroupMatesHelper::AddCharacter(FPawnType* PCPtr)
{
	MembersSet.Add(PCPtr);

	PCPtr->GetGroupMnaggerComponent()->OnAddToNewGroup(OwnerPtr);

	MembersChanged.ExcuteCallback(EGroupMateChangeType::kAdd, PCPtr);
}

void FTeamMatesHelper::AddCharacter(UCharacterUnit* GourpMateUnitPtr, FPawnType* PCPtr)
{
	MembersMap.Add(GourpMateUnitPtr, PCPtr);

	PCPtr->GetGroupMnaggerComponent()->OnAddToNewTeam(OwnerPtr);

	MembersChanged.ExcuteCallback(EGroupMateChangeType::kAdd, PCPtr);
}

void FTeamMatesHelper::SwitchTeammateOption(ETeammateOption InTeammateOption)
{
	TeammateOption = InTeammateOption;

	TeammateOptionChanged.ExcuteCallback(InTeammateOption, OwnerPtr);
}

ETeammateOption FTeamMatesHelper::GetTeammateOption() const
{
	return TeammateOption;
}
