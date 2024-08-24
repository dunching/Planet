// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"

#include "GenerateType.h"

#include "STT_UpdateIsFarawayOriginal.generated.h"

class IGameplayTaskOwnerInterface;

class UAITask_SwitchWalkState;

class AHumanCharacter;
class AHumanAIController;
class UGloabVariable;

USTRUCT()
struct PLANET_API FStateTreeUpdateIsFarawayOriginalTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable* GloabVariable = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

USTRUCT()
struct PLANET_API FSTT_UpdateIsFarawayOriginal : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeUpdateIsFarawayOriginalTaskInstanceData;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context, 
		const FStateTreeTransitionResult& Transition
	) const override;

};
