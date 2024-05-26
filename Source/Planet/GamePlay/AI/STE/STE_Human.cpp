
#include "STE_Human.h"

#include <NavigationSystem.h>

#include "HumanCharacter.h"
#include "GroupMnaggerComponent.h"
#include "HumanAIController.h"

void USTE_Human::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable = NewObject<UGloabVariable>();

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

void USTE_Human::TreeStop(FStateTreeExecutionContext& Context)
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

void USTE_Human::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
}

void USTE_Human::OnTeamOptionChanged(ETeammateOption NewTeammateOption)
{
	TeammateOption = NewTeammateOption;
}

void USTE_Human::OnTeamChanged()
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
