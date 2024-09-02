
#include "STE_Human.h"

#include <NavigationSystem.h>
#include <Perception/AIPerceptionComponent.h>
#include <Components/SphereComponent.h>
#include <Components/SplineComponent.h>

#include "HumanCharacter.h"
#include "GroupMnaggerComponent.h"
#include "HumanAIController.h"
#include "PlanetPlayerController.h"
#include "LogWriter.h"
#include "BuildingArea.h"

void USTE_Human::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable = NewObject<UGloabVariable>();

	if (HumanAIControllerPtr)
	{
		KownCharacterChangedHandle =
			HumanCharacterPtr->GetGroupMnaggerComponent()->GetTeamHelper()->KnowCharaterChanged.AddCallback(
				std::bind(&ThisClass::KnowCharaterChanged, this, std::placeholders::_1, std::placeholders::_2)
			);

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

	switch (TeammateOption)
	{
	case ETeammateOption::kEnemy:
	{
		FTSTicker::GetCoreTicker().RemoveTicker(CaculationDistance2AreaHandle);
	}
	break;
	case ETeammateOption::kFollow:
	{
	}
	break;
	case ETeammateOption::kAssistance:
	{
	}
	break;
	}
}

void USTE_Human::OnTeamChanged()
{
	auto TeamHelperSPtr = HumanCharacterPtr->GetGroupMnaggerComponent()->GetTeamHelper();
	if (TeamHelperSPtr)
	{
		LeaderCharacterPtr = TeamHelperSPtr->OwnerCharacterUnitPtr->ProxyCharacterPtr.Get();

		TeammateOptionChangedDelegate = TeamHelperSPtr->TeammateOptionChanged.AddCallback(
			std::bind(&ThisClass::OnTeamOptionChanged, this, std::placeholders::_1)
		);
		OnTeamOptionChanged(TeamHelperSPtr->GetTeammateOption());
	}
}

void USTE_Human::KnowCharaterChanged(TWeakObjectPtr<ACharacterBase> KnowCharacter, bool bIsAdd)
{
}
