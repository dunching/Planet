// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "AITypes.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"

#include "BTTask_ReleaseSkill.generated.h"

class UAITask_ReleaseSkill;
class IPlanetControllerInterface;
class ACharacterBase;

struct FBTReleaseSkillTaskMemory
{
	IPlanetControllerInterface * PCPtr = nullptr;
	
	ACharacterBase * CharacterPtr = nullptr;

	TWeakObjectPtr<UAITask_ReleaseSkill> Task;
};

UCLASS()
class PLANET_API UBTTask_ReleaseSkill : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:

	using FTaskMemoryType = FBTReleaseSkillTaskMemory;

	using FAITaskType = UAITask_ReleaseSkill;

	UBTTask_ReleaseSkill(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory
	) override;

	virtual EBTNodeResult::Type AbortTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory
	) override;

	virtual uint16 GetInstanceMemorySize() const override;

	virtual void InitializeMemory(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
		EBTMemoryInit::Type InitType
	) const override;

	virtual void CleanupMemory(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
		EBTMemoryClear::Type CleanupType
	) const override;

protected:

	virtual EBTNodeResult::Type PerformMoveTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory
	);

	virtual FAITaskType* PrepareMoveTask(
		UBehaviorTreeComponent& OwnerComp, FAITaskType* ExistingTask
	);

};