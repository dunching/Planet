
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
#include "HoldingItemsComponent.h"
#include "SceneUnitContainer.h"

FName UGroupMnaggerComponent::ComponentName = TEXT("GroupMnaggerComponent");

void UGroupMnaggerComponent::AddCharacterToGroup(FPawnType* TargetCharaterPtr)
{
	// 
	auto CharacterPtr = GetOwner<FOwnerType>()->GetRealCharacter();
	if (CharacterPtr && CharacterPtr->IsPlayerControlled())
	{
		auto AIControllerPtr = TargetCharaterPtr->GetController<APlanetAIController>(); 

		CharacterPtr->GetHoldingItemsComponent()->GetSceneUnitContainer()->AddUnit_Groupmate(TargetCharaterPtr->GetGourpMateUnit());

		// 
		TargetCharaterPtr->GetHoldingItemsComponent()->GetSceneUnitContainer()->RemoveUnit(TargetCharaterPtr->GetGourpMateUnit()->GetID());
	}

	//
	GetGroupHelper()->AddCharacter(TargetCharaterPtr);
}

void UGroupMnaggerComponent::AddCharacterToTeam(FPawnType* TargetCharaterPtr)
{
	GetGroupHelper()->AddCharacter(TargetCharaterPtr);
}

void UGroupMnaggerComponent::OnAddToNewGroup(FPawnType* TargetCharaterPtr)
{
	GroupHelperSPtr = TargetCharaterPtr->GetGroupMnaggerComponent()->GetGroupHelper();

	if (TargetCharaterPtr->IsPlayerControlled())
	{
		TeamHelperSPtr->SwitchTeammateOption(ETeammateOption::kFree);
	}

	GroupHelperChangedDelegateContainer.ExcuteCallback();
}

void UGroupMnaggerComponent::OnAddToNewTeam(FPawnType* TargetCharaterPtr)
{
	TeamHelperSPtr = TargetCharaterPtr->GetGroupMnaggerComponent()->GetTeamHelper();

	TeamHelperChangedDelegateContainer.ExcuteCallback();
}

const TSharedPtr<FGroupMatesHelper>& UGroupMnaggerComponent::GetGroupHelper() 
{
	if (!GroupHelperSPtr)
	{
		auto TargetCharaterPtr = GetOwner<FOwnerType>();
		GroupHelperSPtr = UGroupsManaggerSubSystem::GetInstance()->CreateGroup(GetOwner<FOwnerType>()->GetRealCharacter());

		GroupHelperChangedDelegateContainer.ExcuteCallback();
	}
	return GroupHelperSPtr;
}

const TSharedPtr<FTeamMatesHelper>& UGroupMnaggerComponent::GetTeamHelper()
{
	if (!TeamHelperSPtr)
	{
		auto TargetCharaterPtr = GetOwner<FOwnerType>();
		TeamHelperSPtr = UGroupsManaggerSubSystem::GetInstance()->CreateTeam(GetOwner<FOwnerType>()->GetRealCharacter());

		TeamHelperChangedDelegateContainer.ExcuteCallback();
	}
	return TeamHelperSPtr;
}

void UGroupMnaggerComponent::BeginPlay()
{
	Super::BeginPlay();
}
