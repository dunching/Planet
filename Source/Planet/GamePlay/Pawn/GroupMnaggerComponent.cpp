
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

void UGroupMnaggerComponent::AddCharacterToGroup(FCharacterUnitType* CharacterUnitPtr)
{
	//
	GetGroupHelper()->AddCharacter(CharacterUnitPtr);
}

void UGroupMnaggerComponent::AddCharacterToTeam(FCharacterUnitType* CharacterUnitPtr)
{
	GetGroupHelper()->AddCharacter(CharacterUnitPtr);
}

void UGroupMnaggerComponent::OnAddToNewGroup(FCharacterUnitType* CharacterUnitPtr)
{
	GroupHelperSPtr = CharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetGroupHelper();

	if (CharacterUnitPtr->ProxyCharacterPtr->IsPlayerControlled())
	{
		TeamHelperSPtr->SwitchTeammateOption(ETeammateOption::kFree);
	}

	GroupHelperChangedDelegateContainer.ExcuteCallback();
}

void UGroupMnaggerComponent::OnAddToNewTeam(FCharacterUnitType* CharacterUnitPtr)
{
	TeamHelperSPtr = CharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetTeamHelper();

	TeamHelperChangedDelegateContainer.ExcuteCallback();
}

const TSharedPtr<FGroupMatesHelper>& UGroupMnaggerComponent::GetGroupHelper()
{
	if (!GroupHelperSPtr)
	{
		CreateGroup();
	}
	return GroupHelperSPtr;
}

const TSharedPtr<FTeamMatesHelper>& UGroupMnaggerComponent::GetTeamHelper()
{
	if (!TeamHelperSPtr)
	{
		CreateTeam();
	}
	return TeamHelperSPtr;
}

void UGroupMnaggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

TSharedPtr<FGroupMatesHelper> UGroupMnaggerComponent::CreateGroup()
{
	GroupHelperSPtr = MakeShared<FGroupMatesHelper>();
	GroupHelperSPtr->OwnerCharacterUnitPtr = GetOwner<IPlanetControllerInterface>()->GetCharacterUnit();
	for (;;)
	{
		GroupHelperSPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		break;
	}

	GroupHelperChangedDelegateContainer.ExcuteCallback();

	return GroupHelperSPtr;
}

TSharedPtr<FTeamMatesHelper> UGroupMnaggerComponent::CreateTeam()
{
	TeamHelperSPtr = MakeShared<FTeamMatesHelper>();
	TeamHelperSPtr->OwnerCharacterUnitPtr = GetOwner<IPlanetControllerInterface>()->GetCharacterUnit();
	for (;;)
	{
		TeamHelperSPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		break;
	}

	TeamHelperChangedDelegateContainer.ExcuteCallback();

	return TeamHelperSPtr;
}

void FGroupMatesHelper::AddCharacter(FPawnType* PCPtr)
{
	auto CharacterUnitPtr = PCPtr->GetCharacterUnit();
	AddCharacter(CharacterUnitPtr);
}

void FGroupMatesHelper::AddCharacter(FCharacterUnitType* CharacterUnitPtr)
{
	MembersSet.Add(CharacterUnitPtr);

	CharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->OnAddToNewGroup(OwnerCharacterUnitPtr);

	MembersChanged.ExcuteCallback(EGroupMateChangeType::kAdd, CharacterUnitPtr);
}

bool FGroupMatesHelper::IsMember(FCharacterUnitType* CharacterUnitPtr) const
{
	for (auto Iter : MembersSet)
	{
		if (Iter == CharacterUnitPtr)
		{
			return true;
		}
	}

	if (CharacterUnitPtr == OwnerCharacterUnitPtr)
	{
		return true;
	}

	return false;
}

void FTeamMatesHelper::SwitchTeammateOption(ETeammateOption InTeammateOption)
{
	TeammateOption = InTeammateOption;

	TeammateOptionChanged.ExcuteCallback(InTeammateOption, OwnerCharacterUnitPtr);
}

ETeammateOption FTeamMatesHelper::GetTeammateOption() const
{
	return TeammateOption;
}
