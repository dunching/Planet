
#include "GroupsManaggerSubSystem.h"

#include "Subsystems/SubsystemBlueprintLibrary.h"

#include "Planet.h"
#include "GroupMnaggerComponent.h"
#include "CharacterBase.h"
#include "SceneElement.h"
#include "HumanControllerInterface.h"
#include "HumanCharacter.h"

UGroupsManaggerSubSystem* UGroupsManaggerSubSystem::GetInstance()
{
	return Cast<UGroupsManaggerSubSystem>(USubsystemBlueprintLibrary::GetGameInstanceSubsystem(GetWorldImp(), UGroupsManaggerSubSystem::StaticClass()));
}

TSharedPtr<UGroupsManaggerSubSystem::FGroupMatesHelper> UGroupsManaggerSubSystem::CreateGroup(FPawnType* PCPtr)
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

TSharedPtr<UGroupsManaggerSubSystem::FTeamMatesHelper> UGroupsManaggerSubSystem::CreateTeam(FPawnType* PCPtr)
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

void UGroupsManaggerSubSystem::FGroupMatesHelper::AddCharacter(FPawnType* PCPtr)
{
	MembersSet.Add(PCPtr);

	PCPtr->GetGroupMnaggerComponent()->OnAddToNewGroup(OwnerPCPtr);

	MembersChanged.ExcuteCallback(EGroupMateChangeType::kAdd, PCPtr);
}

void UGroupsManaggerSubSystem::FTeamMatesHelper::AddCharacter(UGourpmateUnit* GourpMateUnitPtr, FPawnType* PCPtr)
{
	MembersMap.Add(GourpMateUnitPtr, PCPtr);

	PCPtr->GetGroupMnaggerComponent()->OnAddToNewTeam(OwnerPCPtr);

	MembersChanged.ExcuteCallback(EGroupMateChangeType::kAdd, PCPtr);
}

void UGroupsManaggerSubSystem::FTeamMatesHelper::SwitchTeammateOption(ETeammateOption InTeammateOption)
{
	TeammateOption = InTeammateOption;

	TeammateOptionChanged.ExcuteCallback(InTeammateOption, OwnerPCPtr);
}

ETeammateOption UGroupsManaggerSubSystem::FTeamMatesHelper::GetTeammateOption() const
{
	return TeammateOption;
}
