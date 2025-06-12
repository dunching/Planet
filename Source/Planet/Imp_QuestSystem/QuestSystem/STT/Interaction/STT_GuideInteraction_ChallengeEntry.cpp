#include "STT_GuideInteraction_ChallengeEntry.h"

#include "AssetRefMap.h"
#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_Player.h"

#include "QuestsActorBase.h"
#include "QuestInteraction.h"
#include "InteractionGameplayTask.h"
#include "QuestSubSystem.h"
#include "GameplayTask_Quest.h"
#include "HumanRegularProcessor.h"
#include "GuideThreadChallenge.h"
#include "STE_Interaction.h"

EStateTreeRunStatus FSTT_GuideInteraction_EntryChallenge::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);
	if (InstanceData.GuideActorPtr)
	{
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}

	UQuestSubSystem::GetInstance()->StartParallelGuideThread(
		UAssetRefMap::GetInstance()->GuideThreadChallengeActorClass
	);
	
	return EStateTreeRunStatus::Succeeded;
}
