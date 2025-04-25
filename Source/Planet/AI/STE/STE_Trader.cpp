
#include "STE_Trader.h"

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

void USTE_Trader::TreeStart(FStateTreeExecutionContext& Context)
{
	Super::TreeStart(Context);

	GetWorld()->GetTimerManager().SetTimer(
		CheckKnowCharacterTimerHandle,
		this,
		&ThisClass::UpdateSensingCharacters,
		1.f,
		true
	);
}

void USTE_Trader::UpdateTargetCharacter(
	FStateTreeExecutionContext& Context
)
{
	Super::UpdateTargetCharacter(Context);
}

void USTE_Trader::UpdateSensingCharacters()
{
	auto TeamMatesHelperComponent= HumanAIControllerPtr->GetGroupSharedInfo()->GetTeamMatesHelperComponent();
	auto KnowCharatersSet = TeamMatesHelperComponent->GetSensingChractersSet();
	auto OwnerCharacterProxyPtr = TeamMatesHelperComponent->GetOwnerCharacterProxyPtr();

	decltype(KnowCharatersSet) NewSensingChractersSet;
	decltype(KnowCharatersSet) NewValidCharater;
	
	// 是否有效
	{
		TSet<ACharacterBase*> NeedRemoveAry;
		for (const auto& Iter : KnowCharatersSet)
		{
			if (Iter.IsValid())
			{
				if (Iter->GetCharacterAbilitySystemComponent()->IsInDeath())
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
	NewSensingChractersSet = KnowCharatersSet;
	TeamMatesHelperComponent->SetSensingChractersSet(NewSensingChractersSet);

	// 是否过远
	auto CharacterActorPtr = OwnerCharacterProxyPtr->GetCharacterActor();
	if (CharacterActorPtr.IsValid() && CharacterActorPtr->IsA(AHumanCharacter_AI::StaticClass()))
	{
		auto AI_CharacterActorPtr = Cast<AHumanCharacter_AI>(CharacterActorPtr.Get());

		const auto Location = AI_CharacterActorPtr->GetActorLocation();

		TSet<ACharacterBase*> NeedRemoveAry;
		for (const auto& Iter : KnowCharatersSet)
		{
			if (Iter.IsValid())
			{
				if (FVector::Distance(Iter->GetActorLocation(), Location) > MaxDistance)
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
	}
	NewValidCharater = KnowCharatersSet;
	TeamMatesHelperComponent->SetValidCharater(NewValidCharater);
}
