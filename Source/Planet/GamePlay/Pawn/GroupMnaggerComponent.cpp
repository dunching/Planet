
#include "GroupMnaggerComponent.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"

#include "GroupsManaggerSubSystem.h"
#include "CharacterBase.h"
#include "HumanCharacter.h"
#include "AIHumanInfo.h"
#include "PlanetAIController.h"
#include "PlanetControllerInterface.h"

FName UGroupMnaggerComponent::ComponentName = TEXT("GroupMnaggerComponent");

void UGroupMnaggerComponent::AddCharacterToGroup(IPlanetControllerInterface* PCPtr)
{
	GetGroupsHelper()->AddCharacter(PCPtr);
}

void UGroupMnaggerComponent::AddCharacterToTeam(IPlanetControllerInterface* PCPtr)
{
	GetGroupsHelper()->AddCharacter(PCPtr);
}

void UGroupMnaggerComponent::OnAddToNewGroup(IPlanetControllerInterface* OwnerPCPtr)
{
	GroupHelperSPtr = OwnerPCPtr->GetGroupMnaggerComponent()->GetGroupsHelper();
}

void UGroupMnaggerComponent::OnAddToNewTeam(IPlanetControllerInterface* OwnerPCPtr)
{
	TeamHelperSPtr = OwnerPCPtr->GetGroupMnaggerComponent()->GetTeamsHelper();
}

const TSharedPtr<UGroupsManaggerSubSystem::FGroupMatesHelper>& UGroupMnaggerComponent::GetGroupsHelper() 
{
	if (!GroupHelperSPtr)
	{
		auto OwnerPCPtr = GetOwner<IPlanetControllerInterface>();
		GroupHelperSPtr = UGroupsManaggerSubSystem::GetInstance()->CreateGroup(OwnerPCPtr);
	}
	return GroupHelperSPtr;
}

const TSharedPtr<UGroupsManaggerSubSystem::FTeamMatesHelper>& UGroupMnaggerComponent::GetTeamsHelper()
{
	if (!TeamHelperSPtr)
	{
		auto OwnerPCPtr = GetOwner<IPlanetControllerInterface>();
		TeamHelperSPtr = UGroupsManaggerSubSystem::GetInstance()->CreateTeam(OwnerPCPtr);
	}
	return TeamHelperSPtr;
}

void UGroupMnaggerComponent::BeginPlay()
{
	Super::BeginPlay();
}
