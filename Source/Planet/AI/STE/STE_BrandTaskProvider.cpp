
#include "STE_BrandTaskProvider.h"

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
#include "HumanCharacter_AI.h"
#include "ItemProxy_Character.h"
#include "STE_CharacterBase.h"

void USTE_BrandTaskProvider::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);
}

void USTE_BrandTaskProvider::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
)
{
	Super::Tick(Context, DeltaTime);
}

UGloabVariable_Character* USTE_BrandTaskProvider::CreateGloabVarianble()
{
	auto GloabVariablePtr = NewObject<FGloabVariable>();

	GloabVariablePtr->UpdateTargetCharacterFunc = std::bind(&ThisClass::UpdateTargetCharacter, this);

	return GloabVariablePtr;
}

TWeakObjectPtr<ACharacterBase> USTE_BrandTaskProvider::UpdateTargetCharacter()
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
