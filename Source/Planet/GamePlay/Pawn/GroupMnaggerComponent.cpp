
#include "GroupMnaggerComponent.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"

#include "GroupsManaggerSubSystem.h"
#include "CharacterBase.h"
#include "HumanCharacter.h"
#include "AIHumanInfo.h"
#include "HumanAIController.h"
#include "HumanControllerInterface.h"

FName UGroupMnaggerComponent::ComponentName = TEXT("GroupMnaggerComponent");

void UGroupMnaggerComponent::AddCharacterToGroup(FPawnType* PCPtr)
{
	GetGroupHelper()->AddCharacter(PCPtr);
}

void UGroupMnaggerComponent::AddCharacterToTeam(FPawnType* PCPtr)
{
	GetGroupHelper()->AddCharacter(PCPtr);
}

void UGroupMnaggerComponent::OnAddToNewGroup(FPawnType* OwnerPtr)
{
	GroupHelperSPtr = OwnerPtr->GetGroupMnaggerComponent()->GetGroupHelper();

	GroupHelperChangedDelegateContainer.ExcuteCallback();
}

void UGroupMnaggerComponent::OnAddToNewTeam(FPawnType* OwnerPtr)
{
	TeamHelperSPtr = OwnerPtr->GetGroupMnaggerComponent()->GetTeamHelper();

	TeamHelperChangedDelegateContainer.ExcuteCallback();
}

const TSharedPtr<UGroupsManaggerSubSystem::FGroupMatesHelper>& UGroupMnaggerComponent::GetGroupHelper() 
{
	if (!GroupHelperSPtr)
	{
		auto OwnerPtr = GetOwner<FPawnType>();
		GroupHelperSPtr = UGroupsManaggerSubSystem::GetInstance()->CreateGroup(OwnerPtr);

		GroupHelperChangedDelegateContainer.ExcuteCallback();
	}
	return GroupHelperSPtr;
}

const TSharedPtr<UGroupsManaggerSubSystem::FTeamMatesHelper>& UGroupMnaggerComponent::GetTeamHelper()
{
	if (!TeamHelperSPtr)
	{
		auto OwnerPtr = GetOwner<FPawnType>();
		TeamHelperSPtr = UGroupsManaggerSubSystem::GetInstance()->CreateTeam(OwnerPtr);

		TeamHelperChangedDelegateContainer.ExcuteCallback();
	}
	return TeamHelperSPtr;
}

void UGroupMnaggerComponent::BeginPlay()
{
	Super::BeginPlay();
}
