
#include "STE_Human.h"

#include <NavigationSystem.h>
#include <Perception/AIPerceptionComponent.h>
#include <Components/SphereComponent.h>

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

	switch (TeammateOption)
	{
	case ETeammateOption::kEnemy:
	{
		FTSTicker::GetCoreTicker().RemoveTicker(CaculationDistance2AreaHandle);
		CaculationDistance2AreaHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::UpdateInArea), 1.f);
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
		if (CharacterPtr->GetController()->IsA(APlanetPlayerController::StaticClass()))
		{
			if (Stimulus.WasSuccessfullySensed())
			{
				GetWorld()->GetTimerManager().ClearTimer(RemoveTarget);
				TargetSet.Add(CharacterPtr);

				PRINTINVOKEWITHSTR(FString(TEXT("Found Target")));
			}
			else
			{
				GetWorld()->GetTimerManager().SetTimer(RemoveTarget, [&, CharacterPtr]() {
					if (TargetSet.Contains(CharacterPtr))
					{
						TargetSet.Remove(CharacterPtr);
					}

					PRINTINVOKEWITHSTR(FString(TEXT("Not Find Target Remove Target")));
					}, 5.f, false, 5.f);

				PRINTINVOKEWITHSTR(FString(TEXT("Not Find Target")));
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

bool USTE_Human::UpdateInArea(float DletaTime)
{
	if (HumanAIControllerPtr->BuildingArea)
	{
		const auto Distance = FVector::Distance(HumanCharacterPtr->GetActorLocation(), HumanAIControllerPtr->BuildingArea->GetActorLocation());
		bIsInArea = Distance < HumanAIControllerPtr->BuildingArea->AreaPtr->GetScaledSphereRadius();

		PRINTINVOKEWITHSTR(FString::Printf(TEXT("Update Distance 2 Area:%.2lf"), Distance));
	}
	return true;
}

void USTE_Human::CaculationPatrolPosition()
{
	if (TargetSet.IsEmpty())
	{
		TargetCharacterPtr = nullptr;
		HumanAIControllerPtr->TargetCharacterPtr = TargetCharacterPtr;
		FTSTicker::GetCoreTicker().RemoveTicker(CaculationPatrolHandle);
		CaculationPatrolHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &ThisClass::GetPatrolPosition), GetPatrolPositionDelta);
	}
	else
	{
		for (auto Iter : TargetSet)
		{
			if (Iter->GetController()->IsA(APlanetPlayerController::StaticClass()))
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
		FTSTicker::GetCoreTicker().RemoveTicker(CaculationPatrolHandle);
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
