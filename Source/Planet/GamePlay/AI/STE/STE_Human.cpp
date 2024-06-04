
#include "STE_Human.h"

#include <NavigationSystem.h>
#include <Perception/AIPerceptionComponent.h>

#include "HumanCharacter.h"
#include "GroupMnaggerComponent.h"
#include "HumanAIController.h"
#include "HumanPlayerController.h"

void USTE_Human::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GloabVariable = NewObject<UGloabVariable>();

	if (HumanAIControllerPtr)
	{
		HumanAIControllerPtr->GetAIPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ThisClass::OnTargetPerceptionUpdated);
		HumanAIControllerPtr->GetAIPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &ThisClass::OnPerceptionUpdated);

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
		TargetCharacterPtr = TeamHelperSPtr->OwnerPtr;

		TeammateOptionChangedDelegate = TeamHelperSPtr->TeammateOptionChanged.AddCallback(
			std::bind(&ThisClass::OnTeamOptionChanged, this, std::placeholders::_1)
		);
		OnTeamOptionChanged(TeamHelperSPtr->GetTeammateOption());
	}
}

void USTE_Human::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		bIsFoundTarget = true;

		FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);

		auto CharacterPtr = Cast<AHumanCharacter>(Actor);
		if (CharacterPtr->GetController()->IsA(AHumanPlayerController::StaticClass()))
		{
			TargetCharacterPtr = CharacterPtr;
		}
		else
		{
			if (!TargetCharacterPtr)
			{
				TargetCharacterPtr = CharacterPtr;
			}
		}
	}
}

void USTE_Human::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	if (UpdatedActors.IsEmpty())
	{
		TargetCharacterPtr = nullptr;

		bIsFoundTarget = false;

		TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::GetPatrolPosition), GetPatrolPositionDelta);
	}
}

bool USTE_Human::GetPatrolPosition(float)
{
	FVector Location = FVector::ZeroVector;
	if (UNavigationSystemV1::K2_GetRandomReachablePointInRadius(
		this,
		HumanCharacterPtr->GetActorLocation(),
		GloabVariable->Location,
		800.f
	))
	{
		GloabVariable->Location = Location;
	}

	return true;
}
