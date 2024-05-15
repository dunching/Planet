
#include "BTTask_ReleaseSkill.h"

#include "GameFramework/Actor.h"
#include "AISystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "NavFilters/NavigationQueryFilter.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "VisualLogger/VisualLogger.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Tasks/AITask_MoveTo.h"

#include "AITask_ReleaseSkill.h"
#include "PlanetControllerInterface.h"
#include "CharacterBase.h"

UBTTask_ReleaseSkill::UBTTask_ReleaseSkill(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	NodeName = "ReleaseSkill";

	INIT_TASK_NODE_NOTIFY_FLAGS();

	// accept only actors and vectors
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, BlackboardKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_ReleaseSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	auto IPCPtr = Cast<IPlanetControllerInterface>(OwnerComp.GetAIOwner());
	AAIController * AICPtr = OwnerComp.GetAIOwner();
	if (IPCPtr == nullptr || AICPtr == nullptr)
	{
		NodeResult = EBTNodeResult::Failed;
	}
	else
	{
		FTaskMemoryType* MyMemory = CastInstanceNodeMemory<FTaskMemoryType>(NodeMemory);

		NodeResult = PerformMoveTask(OwnerComp, NodeMemory);
	}

	return NodeResult;
}

EBTNodeResult::Type UBTTask_ReleaseSkill::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FTaskMemoryType* MyMemory = CastInstanceNodeMemory<FTaskMemoryType>(NodeMemory);

	auto MoveTask = MyMemory->Task.Get();
	if (MoveTask)
	{
		MoveTask->ExternalCancel();
	}

	return Super::AbortTask(OwnerComp, NodeMemory);
}

uint16 UBTTask_ReleaseSkill::GetInstanceMemorySize() const
{
	return sizeof(FTaskMemoryType);
}

void UBTTask_ReleaseSkill::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	InitializeNodeMemory<FTaskMemoryType>(NodeMemory, InitType);
}

void UBTTask_ReleaseSkill::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FTaskMemoryType>(NodeMemory, CleanupType);
}

EBTNodeResult::Type UBTTask_ReleaseSkill::PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	FTaskMemoryType* MyMemory = CastInstanceNodeMemory<FTaskMemoryType>(NodeMemory);

	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;
	if (MyBlackboard)
	{
		FAITaskType* MoveTask = MyMemory->Task.Get();
		const bool bReuseExistingTask = (MoveTask != nullptr);

		MoveTask = PrepareMoveTask(OwnerComp, MoveTask);
		if (MoveTask)
		{
			if (bReuseExistingTask)
			{
				if (MoveTask->IsActive())
				{
					MoveTask->ConditionalPerformMove();
				}
				else
				{
				}
			}
			else
			{
				MyMemory->Task = MoveTask;
				MoveTask->ReadyForActivation();
			}

			NodeResult = (MoveTask->GetState() != EGameplayTaskState::Finished) ? EBTNodeResult::InProgress :
				MoveTask->WasMoveSuccessful() ? EBTNodeResult::Succeeded :
				EBTNodeResult::Failed;
		}
	}

	return NodeResult;
}

UBTTask_ReleaseSkill::FAITaskType* UBTTask_ReleaseSkill::PrepareMoveTask(UBehaviorTreeComponent& OwnerComp, FAITaskType* ExistingTask)
{
	FAITaskType* MoveTask = ExistingTask ? ExistingTask : NewBTAITask<FAITaskType>(OwnerComp);
	if (MoveTask)
	{
		const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
			ACharacterBase* TargetActor = Cast<ACharacterBase>(KeyValue);
			if (TargetActor)
			{
				MoveTask->SetUp(TargetActor);
			}
		}
		else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
		}
	}

	return MoveTask;
}

