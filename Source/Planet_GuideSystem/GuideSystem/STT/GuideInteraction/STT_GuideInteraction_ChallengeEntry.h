// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateTypes.h"
#include "STT_GuideBase.h"
#include "STT_CommonData.h"
#include "GuideActorBase.h"
#include "STT_GuideInteraction.h"

#include "STT_GuideInteraction_ChallengeEntry.generated.h"

class AGuideActor;
class AGuideThread;
class AGuideThread_Main;
class AGuideInteractionBase;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;
class UGloabVariable_Interaction;
class UGameplayTask_Base;
class UGameplayTask_Interaction_Conversation;
class UGameplayTask_Interaction_Option;
class UGameplayTask_Interaction_NotifyGuideThread;

#pragma region 进入挑战
USTRUCT()
struct PLANET_GUIDESYSTEM_API FSTID_GuideInteraction_EntryChallenge :
	public FSTID_GuideInteractionTask
{
	GENERATED_BODY()

};

USTRUCT()
struct PLANET_GUIDESYSTEM_API FSTT_GuideInteraction_EntryChallenge :
	public FSTT_GuideInteraction
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_GuideInteraction_EntryChallenge;

	virtual const UStruct* GetInstanceDataType() const override
	{
		return FInstanceDataType::StaticStruct();
	};

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;
};
#pragma endregion
