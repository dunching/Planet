// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"

#include "GenerateType.h"
#include "STT_CharacterBase.h"

#include "STT_UpdateQueryDistance.generated.h"

class IGameplayTaskOwnerInterface;

class UAITask_SwitchWalkState;

class AHumanCharacter;
class AHumanAIController;
class UGloabVariable_Character;
class USTE_Assistance;

USTRUCT()
struct PLANET_API FSTID_UpdateQueryDistance : public FSTID_CharacterBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	UGloabVariable_Character* GloabVariable = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Param)
	bool bRunForever = true;
	
	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

USTRUCT()
struct PLANET_API FSTT_UpdateQueryDistance : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FSTID_UpdateQueryDistance;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context, 
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context,
		const float DeltaTime
	) const override;
	
	void PerformAction(FStateTreeExecutionContext& Context)const;
	
};
