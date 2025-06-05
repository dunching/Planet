// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"

#include "GenerateTypes.h"
#include "STT_CharacterBase.h"

#include "STT_UpdateTargetCharacter.generated.h"

class IGameplayTaskOwnerInterface;

class UAITask_SwitchWalkState;

class AHumanCharacter;
class AHumanCharacter_AI;
class AHumanAIController;
class UGloabVariable_Character;
class USTE_Assistance;

USTRUCT()
struct PLANET_API FSTID_UpdateTargetCharacter : public FSTID_CharacterBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_Character* GloabVariable_Character = nullptr;

	UPROPERTY(EditAnywhere, Category = Param)
	bool bRunForever = false;

	UPROPERTY(EditAnywhere, Category = Param)
	bool bCheckHave = true;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

USTRUCT()
struct PLANET_API FSTT_UpdateTargetCharacter : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_UpdateTargetCharacter;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;

	virtual EStateTreeRunStatus PerformGameplayTask(FStateTreeExecutionContext& Context) const;

	/**
	 * OnlyServer
	 */
	void UpdateTargetCharacter(FStateTreeExecutionContext& Context)const;
	
};
