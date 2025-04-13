#include "STT_GuideInteraction_ChallengeEntry.h"

#include "AssetRefMap.h"
#include "Kismet/GameplayStatics.h"

#include "HumanCharacter_Player.h"

#include "GuideActor.h"
#include "GuideInteraction.h"
#include "GuideInteractionGameplayTask.h"
#include "GuideSubSystem.h"
#include "GuideSystemGameplayTask.h"
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

	UGuideSubSystem::GetInstance()->ActiveTargetGuideThread(
		UAssetRefMap::GetInstance()->GuideThreadChallengeActorClass,
		true
	);
	
	return EStateTreeRunStatus::Succeeded;
}
