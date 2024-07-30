
#include "GroupMnaggerComponent.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"

#include "GroupsManaggerSubSystem.h"
#include "CharacterBase.h"
#include "HumanCharacter.h"
#include "CharacterTitle.h"
#include "HumanAIController.h"
#include "PlanetControllerInterface.h"
#include "PlanetPlayerState.h"

FName UGroupMnaggerComponent::ComponentName = TEXT("GroupMnaggerComponent");

void UGroupMnaggerComponent::AddCharacterToGroup(FPawnType* PCPtr)
{
	// 
	auto CharacterPtr = GetOwner<FPawnType>();
	if (CharacterPtr && CharacterPtr->IsPlayerControlled())
	{
		auto AIControllerPtr = PCPtr->GetController<APlanetAIController>();

		auto GroupmateUnitPtr = 
			CharacterPtr->GetPlayerState<APlanetPlayerState>()->GetSceneUnitContainer().AddUnit_Groupmate(AIControllerPtr->RowName);

		AIControllerPtr->GourpMateUnitPtr = GroupmateUnitPtr;
	}

	//
	GetGroupHelper()->AddCharacter(PCPtr);
}

void UGroupMnaggerComponent::AddCharacterToTeam(FPawnType* PCPtr)
{
	GetGroupHelper()->AddCharacter(PCPtr);
}

void UGroupMnaggerComponent::OnAddToNewGroup(FPawnType* OwnerPtr)
{
	GroupHelperSPtr = OwnerPtr->GetGroupMnaggerComponent()->GetGroupHelper();

	if (OwnerPtr->IsPlayerControlled())
	{
		TeamHelperSPtr->SwitchTeammateOption(ETeammateOption::kFree);
	}

	GroupHelperChangedDelegateContainer.ExcuteCallback();
}

void UGroupMnaggerComponent::OnAddToNewTeam(FPawnType* OwnerPtr)
{
	TeamHelperSPtr = OwnerPtr->GetGroupMnaggerComponent()->GetTeamHelper();

	TeamHelperChangedDelegateContainer.ExcuteCallback();
}

const TSharedPtr<FGroupMatesHelper>& UGroupMnaggerComponent::GetGroupHelper() 
{
	if (!GroupHelperSPtr)
	{
		auto OwnerPtr = GetOwner<FPawnType>();
		GroupHelperSPtr = UGroupsManaggerSubSystem::GetInstance()->CreateGroup(OwnerPtr);

		GroupHelperChangedDelegateContainer.ExcuteCallback();
	}
	return GroupHelperSPtr;
}

const TSharedPtr<FTeamMatesHelper>& UGroupMnaggerComponent::GetTeamHelper()
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
