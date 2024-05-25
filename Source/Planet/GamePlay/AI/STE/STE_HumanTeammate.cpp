
#include "STE_HumanTeammate.h"

#include <NavigationSystem.h>

#include "HumanCharacter.h"
#include "GroupMnaggerComponent.h"
#include "HumanAIController.h"

void USTE_HumanTeammate::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	if (HumanAIControllerPtr)
	{
		HumanCharacterPtr = HumanAIControllerPtr->GetPawn<AHumanCharacter>();
		if (HumanCharacterPtr)
		{
			TeammateChangedDelegate = HumanCharacterPtr->GetGroupMnaggerComponent()->TeamHelperChangedDelegateContainer.AddCallback(
				std::bind(&ThisClass::OnTeamChanged, this)
			);
			OnTeamChanged();
		}
	}
}

void USTE_HumanTeammate::TreeStop(FStateTreeExecutionContext& Context)
{
	if (TeammateOptionChangedDelegate)
	{
		TeammateOptionChangedDelegate->UnBindCallback();
	}

	if (TeammateChangedDelegate)
	{
		TeammateChangedDelegate->UnBindCallback();
	}

	Super::TreeStop(Context);
}

void USTE_HumanTeammate::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
}

void USTE_HumanTeammate::OnTeamOptionChanged(ETeammateOption NewTeammateOption)
{
	TeammateOption = NewTeammateOption;
}

void USTE_HumanTeammate::OnTeamChanged()
{
	auto TeamHelperSPtr = HumanCharacterPtr->GetGroupMnaggerComponent()->GetTeamHelper();
	if (TeamHelperSPtr)
	{
		TargetCharacterPtr = TeamHelperSPtr->OwnerPCPtr;

		TeammateOptionChangedDelegate = TeamHelperSPtr->TeammateOptionChanged.AddCallback(
			std::bind(&ThisClass::OnTeamOptionChanged, this, std::placeholders::_1)
		);
		OnTeamOptionChanged(TeamHelperSPtr->GetTeammateOption());
	}
}
