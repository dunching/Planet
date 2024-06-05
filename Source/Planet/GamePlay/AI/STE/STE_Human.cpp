
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
		
		CaculationPatrolPosition();
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
		LeaderCharacterPtr = TeamHelperSPtr->OwnerPtr;

		TeammateOptionChangedDelegate = TeamHelperSPtr->TeammateOptionChanged.AddCallback(
			std::bind(&ThisClass::OnTeamOptionChanged, this, std::placeholders::_1)
		);
		OnTeamOptionChanged(TeamHelperSPtr->GetTeammateOption());
	}
}

void USTE_Human::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	auto CharacterPtr = Cast<AHumanCharacter>(Actor);
	if (CharacterPtr)
	{
		if (CharacterPtr->GetController()->IsA(AHumanPlayerController::StaticClass()))
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				TargetSet.Add(CharacterPtr);
			}
			else
			{
				if (TargetSet.Contains(CharacterPtr))
				{
					TargetSet.Remove(CharacterPtr);
				}
			}
		}
		else if (CharacterPtr->GetController()->IsA(AHumanAIController::StaticClass()))
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				HumanCharacterPtr->GetGroupMnaggerComponent()->TargetSet.Add(CharacterPtr);
			}
			else
			{
				if (HumanCharacterPtr->GetGroupMnaggerComponent()->TargetSet.Contains(CharacterPtr))
				{
					HumanCharacterPtr->GetGroupMnaggerComponent()->TargetSet.Remove(CharacterPtr);
				}
			}
		}
		CaculationPatrolPosition();
	}
}

void USTE_Human::CaculationPatrolPosition()
{
	if (TargetSet.IsEmpty())
	{
		TargetCharacterPtr = nullptr;
		HumanAIControllerPtr->TargetCharacterPtr = TargetCharacterPtr;
		FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
		TickDelegateHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::GetPatrolPosition), GetPatrolPositionDelta);
	}
	else
	{
		for (auto Iter : TargetSet)
		{
			if (Iter->GetController()->IsA(AHumanPlayerController::StaticClass()))
			{
				TargetCharacterPtr = Iter;
				break;
			}
			else if (Iter->GetController()->IsA(AHumanAIController::StaticClass()))
			{
				TargetCharacterPtr = Iter;
			}
		}
		HumanAIControllerPtr->TargetCharacterPtr = TargetCharacterPtr;
		FTSTicker::GetCoreTicker().RemoveTicker(TickDelegateHandle);
	}
}

void USTE_Human::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
}

bool USTE_Human::GetPatrolPosition(float)
{
	FVector Location = FVector::ZeroVector;
	if (UNavigationSystemV1::K2_GetRandomReachablePointInRadius(
		this,
		HumanCharacterPtr->GetActorLocation(),
		Location,
		800.f
	))
	{
		GloabVariable->Location = Location;
	}

	return true;
}
