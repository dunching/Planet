// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"
#include "GuideThreadActor.h"

#include "STT_GuideBase.generated.h"

class AGuideActor;
class AGuideThread;
class AGuideMainThread;
class AGuideInteractionActor;
class UPAD_TaskNode_Guide;
class UPAD_TaskNode_Interaction;
class ACharacterBase;
class AHumanCharacter;
class AHumanCharacter_Player;
class UGameplayTask_Base;

USTRUCT()
struct PLANET_API FSTID_GuideBase
{
	GENERATED_BODY()

	FSTID_GuideBase();
	
	UPROPERTY(VisibleAnywhere, Category = Output)
	FGuid TaskID;
	
	// 上条任务的输出参数
	UPROPERTY(EditAnywhere, Category = Output)
	FTaskNodeResuleHelper TaskNodeResuleHelper;

};

template<>
struct TStructOpsTypeTraits<FSTID_GuideBase> :
	public TStructOpsTypeTraitsBase2<FSTID_GuideBase>
{
	enum
	{
		WithZeroConstructor = true,
	};
};
