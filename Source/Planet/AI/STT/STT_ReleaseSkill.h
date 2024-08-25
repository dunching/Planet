// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SubclassOf.h"

#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "Tasks/StateTreeAITask.h"

#include "GenerateType.h"

#include "STT_ReleaseSkill.generated.h"

class AHumanCharacter;
class AHumanAIController;
class UAITask_ReleaseSkill;

UENUM(BlueprintType)
enum class EUpdateReleaseSkillStuteType : uint8
{
	kNone,
	kCheck,
	kRelease,
};

UCLASS(Blueprintable)
class PLANET_API UReleaseSkillGloabVariable : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	bool bIsNeedRelease = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = Output)
	EUpdateReleaseSkillStuteType UpdateReleaseSkillStuteType = EUpdateReleaseSkillStuteType::kNone;

};

USTRUCT()
struct PLANET_API FStateTreeReleaseSkillTaskInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Context)
	UReleaseSkillGloabVariable* GloabVariable = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UAITask_ReleaseSkill> AITaskPtr = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

USTRUCT()
struct PLANET_API FSTT_ReleaseSkill : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeReleaseSkillTaskInstanceData;

	using FAITaskType = UAITask_ReleaseSkill;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context, 
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual void ExitState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

	virtual EStateTreeRunStatus Tick(
		FStateTreeExecutionContext& Context, 
		const float DeltaTime
	) const override;

	virtual EStateTreeRunStatus PerformMoveTask(FStateTreeExecutionContext& Context) const;

};

USTRUCT()
struct PLANET_API FStateTreeUpdateReleaseSkillStuteTaskInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanCharacter> CharacterPtr = nullptr;

	UPROPERTY(EditAnywhere, Category = Context)
	TObjectPtr<AHumanAIController> AIControllerPtr = nullptr;
	
	UPROPERTY(EditAnywhere, Category = Context)
	UReleaseSkillGloabVariable* GloabVariable = nullptr;

	UPROPERTY(Transient)
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner = nullptr;
};

USTRUCT()
struct PLANET_API FSTT_UpdateReleaseSkillStuta : public FStateTreeAIActionTaskBase
{
	GENERATED_BODY()

	using FInstanceDataType = FStateTreeUpdateReleaseSkillStuteTaskInstanceData;

	using FAITaskType = UAITask_ReleaseSkill;

	virtual const UStruct* GetInstanceDataType() const override { return FInstanceDataType::StaticStruct(); }

	virtual EStateTreeRunStatus EnterState(
		FStateTreeExecutionContext& Context,
		const FStateTreeTransitionResult& Transition
	) const override;

};
