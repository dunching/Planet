
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
	GroupsHelperSPtr = OwnerPCPtr->GetGroupMnaggerComponent()->GetGroupsHelper();
}

const TSharedPtr<UGroupsManaggerSubSystem::FGroupMatesHelper>& UGroupMnaggerComponent::GetGroupsHelper() 
{
	if (!GroupsHelperSPtr)
	{
		auto OwnerPCPtr = GetOwner<IPlanetControllerInterface>();
		GroupsHelperSPtr = UGroupsManaggerSubSystem::GetInstance()->CreateGroup(OwnerPCPtr);
	}
	return GroupsHelperSPtr;
}

const TSharedPtr<UGroupsManaggerSubSystem::FTeamMatesHelper>& UGroupMnaggerComponent::GetTeamsHelper()
{
	if (!TeamsHelperSPtr)
	{
		auto OwnerPCPtr = GetOwner<IPlanetControllerInterface>();
		TeamsHelperSPtr = UGroupsManaggerSubSystem::GetInstance()->CreateTeam(OwnerPCPtr);
	}
	return TeamsHelperSPtr;
}

void UGroupMnaggerComponent::BeginPlay()
{
	Super::BeginPlay();
}
