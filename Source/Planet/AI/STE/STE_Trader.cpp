#include "STE_Trader.h"

#include <NavigationSystem.h>
#include <Perception/AIPerceptionComponent.h>
#include <Components/SphereComponent.h>
#include <Components/SplineComponent.h>

#include "AIComponent.h"
#include "HumanCharacter.h"
#include "TeamMatesHelperComponentBase.h"
#include "HumanAIController.h"
#include "PlanetPlayerController.h"
#include "LogWriter.h"
#include "BuildingArea.h"
#include "CharacterAbilitySystemComponent.h"
#include "GeneratorColony_ByInvoke.h"
#include "GroupManagger.h"
#include "HumanCharacter_AI.h"
#include "ItemProxy_Character.h"
#include "STE_CharacterBase.h"
#include "TeamMatesHelperComponent.h"

void USTE_Trader::TreeStart(
	FStateTreeExecutionContext& Context
)
{
	Super::TreeStart(Context);
}

UGloabVariable_Character* USTE_Trader::CreateGloabVarianble()
{
	auto GloabVariablePtr = NewObject<FGloabVariable>();

	GloabVariablePtr->UpdateTargetCharacterFunc = std::bind(&ThisClass::UpdateTargetCharacter, this);

	return GloabVariablePtr;
}

TWeakObjectPtr<ACharacterBase> USTE_Trader::UpdateTargetCharacter()
{
	auto TeamMatesHelperComponentPtr = HumanAIControllerPtr->GetGroupManagger()->GetTeamMatesHelperComponent();
	auto KnowCharatersSet = TeamMatesHelperComponentPtr->GetSensingChractersSet();
	
	const auto Location = HumanCharacterPtr->GetActorLocation();

	// 是否过远
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
		}
	}
		
	for (const auto& Iter : NeedRemoveAry)
	{
		if (KnowCharatersSet.Contains(Iter))
		{
			KnowCharatersSet.Remove(Iter);
		}
	}

	return GetNewTargetCharacter(KnowCharatersSet);
}
