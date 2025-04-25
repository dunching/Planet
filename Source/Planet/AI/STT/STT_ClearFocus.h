// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"
#include <EnvironmentQuery/EnvQueryTypes.h>

#include "GenerateType.h"

#include "STT_ClearFocus.generated.h"

class UEnvQuery;

class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanAIController;
class UAITask_ReleaseSkill;
class USTE_Assistance;
class UGloabVariable_Character;

USTRUCT()
struct PLANET_API FSTID_ClearFocus
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter_AI> CharacterPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;

};

USTRUCT()
struct PLANET_API FSTT_ClearFocus : public FStateTreeAIActionTaskBase//FStateTreeTaskCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_ClearFocus;

	virtual const UStruct* GetInstanceDataType() const override {return FInstanceDataType::StaticStruct();};

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

};
