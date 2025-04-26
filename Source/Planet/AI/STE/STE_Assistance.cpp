#include "STE_Assistance.h"

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
#include "CharacterAbilitySystemComponent.h"
#include "GeneratorColony_ByInvoke.h"
#include "GroupManagger.h"
#include "GroupManagger_NPC.h"
#include "HumanCharacter_AI.h"
#include "ItemProxy_Character.h"
#include "STE_CharacterBase.h"

void USTE_Assistance::TreeStart(
	FStateTreeExecutionContext& Context
)
{
	Super::TreeStart(Context);

	GeneratorNPCs_PatrolPtr = HumanCharacterPtr->GetGroupManagger_NPC()->GeneratorNPCs_PatrolPtr;

	if (HumanAIControllerPtr)
	{
		HumanCharacterPtr = HumanAIControllerPtr->GetPawn<AHumanCharacter_AI>();
		if (HumanCharacterPtr)
		{
			TeammateChangedDelegate = HumanCharacterPtr->GetGroupManagger()->GetTeamMatesHelperComponent()->
			                                             TeamHelperChangedDelegateContainer.AddCallback(
				                                             std::bind(&ThisClass::OnTeamChanged, this)
			                                             );
			OnTeamChanged();
		}
	}
}

void USTE_Assistance::TreeStop(
	FStateTreeExecutionContext& Context
)
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

void USTE_Assistance::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
)
{
	Super::Tick(Context, DeltaTime);
}

void USTE_Assistance::OnTeamOptionChanged(
	ETeammateOption NewTeammateOption
)
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
	default: ;
	}
}

void USTE_Assistance::OnTeamChanged()
{
	auto TeamHelperSPtr = HumanCharacterPtr->GetGroupManagger()->GetTeamMatesHelperComponent();
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

UGloabVariable_Character* USTE_Assistance::CreateGloabVarianble()
{
	auto GloabVariablePtr = NewObject<FGloabVariable>();

	GloabVariablePtr->UpdateTargetCharacterFunc = std::bind(&ThisClass::UpdateTargetCharacter, this);

	return GloabVariablePtr;
}

TWeakObjectPtr<ACharacterBase> USTE_Assistance::UpdateTargetCharacter()
{
	auto TeamMatesHelperComponent = HumanAIControllerPtr->GetGroupManagger()->GetTeamMatesHelperComponent();
	auto KnowCharatersSet = TeamMatesHelperComponent->GetSensingChractersSet();
	auto OwnerCharacterProxyPtr = TeamMatesHelperComponent->GetOwnerCharacterProxyPtr();

	// 是否远离了设定位置
	auto CharacterActorPtr = OwnerCharacterProxyPtr->GetCharacterActor();
	if (CharacterActorPtr.IsValid() && CharacterActorPtr->IsA(AHumanCharacter_AI::StaticClass()))
	{
		auto AI_CharacterActorPtr = Cast<AHumanCharacter_AI>(CharacterActorPtr.Get());

		if (GeneratorNPCs_PatrolPtr)
		{
			const auto Location = GeneratorNPCs_PatrolPtr->GetActorLocation();
			const auto MaxDistance = GeneratorNPCs_PatrolPtr->MaxDistance;

			TSet<ACharacterBase*> NeedRemoveAry;
			for (const auto& Iter : KnowCharatersSet)
			{
				if (Iter.IsValid())
				{
					if (FVector::Distance(Iter->GetActorLocation(), Location) > MaxDistance)
					{
						NeedRemoveAry.Add(Iter.Get());
					}
					else if (Iter->IsA(AHumanCharacter_AI::StaticClass()))
					{
						NeedRemoveAry.Add(Iter.Get());
					}
				}
				else
				{
					NeedRemoveAry.Add(Iter.Get());
				}
			}
			for (const auto& Iter : NeedRemoveAry)
			{
				if (KnowCharatersSet.Contains(Iter))
				{
					KnowCharatersSet.Remove(Iter);
				}
			}
		}

		return GetNewTargetCharacter(KnowCharatersSet);
	}

	return nullptr;
}
