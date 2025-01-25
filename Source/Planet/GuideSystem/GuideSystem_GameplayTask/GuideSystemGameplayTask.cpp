#include "GuideSystemGameplayTask.h"

#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "ConversationLayout.h"
#include "GuideActor.h"
#include "GuideSubSystem.h"
#include "HumanCharacter_AI.h"
#include "HumanCharacter_Player.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "OptionList.h"
#include "PlanetPlayerController.h"
#include "TargetPoint_Runtime.h"
#include "TaskNode_Guide.h"
#include "TaskNode_Interaction.h"

class AMainHUD;

void UGameplayTask_Base::SetPlayerCharacter(AHumanCharacter_Player* InPlayerCharacterPtr)
{
	PlayerCharacterPtr = InPlayerCharacterPtr;
}

void UGameplayTask_Base::SetTaskID(const FGuid& InTaskID)
{
	TaskID = InTaskID;
}

void UGameplayTask_Base::SetGuideActor(TObjectPtr<AGuideThread> InGuideActorPtr)
{
	GuideActorPtr = InGuideActorPtr;
}

EStateTreeRunStatus UGameplayTask_Base::GetStateTreeRunStatus() const
{
	return StateTreeRunStatus;
}
