
#include "STE_AICharacterController.h"

#include <NavigationSystem.h>
#include <Perception/AIPerceptionComponent.h>
#include <Components/SphereComponent.h>
#include <Components/SplineComponent.h>

#include "AIComponent.h"
#include "HumanCharacter.h"
#include "TeamMatesHelperComponent.h"
#include "HumanAIController.h"
#include "PlanetPlayerController.h"
#include "LogWriter.h"
#include "BuildingArea.h"
#include "GroupManagger.h"
#include "HumanCharacter_AI.h"
#include "ItemProxy_Character.h"

void USTE_AICharacterController::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable = NewObject<UGloabVariable>();

	GeneratorNPCs_PatrolPtr = HumanCharacterPtr->GetAIComponent()->GeneratorNPCs_PatrolPtr;
	
	if (HumanAIControllerPtr)
	{
		KownCharacterChangedHandle =
			HumanCharacterPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent()->KnowCharaterChanged.AddCallback(
				std::bind(&ThisClass::KnowCharaterChanged, this, std::placeholders::_1, std::placeholders::_2)
			);

		HumanCharacterPtr = HumanAIControllerPtr->GetPawn<AHumanCharacter_AI>();
		if (HumanCharacterPtr)
		{
			TeammateChangedDelegate = HumanCharacterPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent()->TeamHelperChangedDelegateContainer.AddCallback(
				std::bind(&ThisClass::OnTeamChanged, this)
			);
			OnTeamChanged();
		}
	}
}

void USTE_AICharacterController::TreeStop(FStateTreeExecutionContext& Context)
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

void USTE_AICharacterController::Tick(FStateTreeExecutionContext& Context, const float DeltaTime)
{
	Super::Tick(Context, DeltaTime);
}

void USTE_AICharacterController::OnTeamOptionChanged(ETeammateOption NewTeammateOption)
{
#if WITH_EDITOR
	// TODO. 满足测试
	auto DefaultTeammateOption = HumanCharacterPtr->GetAIComponent()->DefaultTeammateOption;
	if (DefaultTeammateOption == ETeammateOption::kTest)
	{
		NewTeammateOption = DefaultTeammateOption;
	}
#endif
	
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

void USTE_AICharacterController::OnTeamChanged()
{
	auto TeamHelperSPtr = HumanCharacterPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent();
	if (TeamHelperSPtr)
	{
		if (auto OwnerCharacterProxySPtr = TeamHelperSPtr->GetOwnerCharacterProxyPtr())
		{
			LeaderCharacterPtr = OwnerCharacterProxySPtr->GetCharacterActor().Get();
		}

		TeammateOptionChangedDelegate = TeamHelperSPtr->TeammateOptionChanged.AddCallback(
			std::bind(&ThisClass::OnTeamOptionChanged, this, std::placeholders::_1)
		);
		OnTeamOptionChanged(TeamHelperSPtr->GetTeammateOption());
	}
}

void USTE_AICharacterController::KnowCharaterChanged(TWeakObjectPtr<ACharacterBase> KnowCharacter, bool bIsAdd)
{
}
