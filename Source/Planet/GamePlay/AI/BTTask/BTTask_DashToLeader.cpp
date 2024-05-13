
#include "BTTask_DashToLeader.h"

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
#include "AITask_DashToLeader.h"

UBTTask_DashToLeader::UBTTask_DashToLeader(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer)
{
	NodeName = "DashToLeader";

	INIT_TASK_NODE_NOTIFY_FLAGS();

	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, BlackboardKey));
}

EBTNodeResult::Type UBTTask_DashToLeader::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	EBTNodeResult::Type NodeResult = EBTNodeResult::InProgress;

	FBTDashToLeaderTaskMemory* MyMemory = CastInstanceNodeMemory<FBTDashToLeaderTaskMemory>(NodeMemory);
	MyMemory->PreviousGoalLocation = FAISystem::InvalidLocation;

	AAIController* MyController = OwnerComp.GetAIOwner();
	if (MyController == nullptr)
	{
		UE_VLOG(OwnerComp.GetOwner(), LogBehaviorTree, Error, TEXT("UBTTask_MoveTo::ExecuteTask failed since AIController is missing."));
		NodeResult = EBTNodeResult::Failed;
	}
	else
	{
		NodeResult = PerformMoveTask(OwnerComp, NodeMemory);
	}

	return NodeResult;
}

EBTNodeResult::Type UBTTask_DashToLeader::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::AbortTask(OwnerComp, NodeMemory);
}

uint16 UBTTask_DashToLeader::GetInstanceMemorySize() const
{
	return sizeof(FBTDashToLeaderTaskMemory);
}

void UBTTask_DashToLeader::InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const
{
	InitializeNodeMemory<FBTDashToLeaderTaskMemory>(NodeMemory, InitType);
}

void UBTTask_DashToLeader::CleanupMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryClear::Type CleanupType) const
{
	CleanupNodeMemory<FBTDashToLeaderTaskMemory>(NodeMemory, CleanupType);
}

EBTNodeResult::Type UBTTask_DashToLeader::PerformMoveTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	FBTDashToLeaderTaskMemory* MyMemory = CastInstanceNodeMemory<FBTDashToLeaderTaskMemory>(NodeMemory);
	AAIController* MyController = OwnerComp.GetAIOwner();

	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;
	if (MyController && MyBlackboard)
	{
		UAITask_DashToLeader* MoveTask = MyMemory->Task.Get();
		const bool bReuseExistingTask = (MoveTask != nullptr);

		MoveTask = PrepareMoveTask(OwnerComp, MoveTask);
		if (MoveTask)
		{
			if (bReuseExistingTask)
			{
				if (MoveTask->IsActive())
				{
					UE_VLOG(MyController, LogBehaviorTree, Verbose, TEXT("\'%s\' reusing AITask %s"), *GetNodeName(), *MoveTask->GetName());
					MoveTask->ConditionalPerformMove();
				}
				else
				{
					UE_VLOG(MyController, LogBehaviorTree, Verbose, TEXT("\'%s\' reusing AITask %s, but task is not active - handing over move performing to task mechanics"), *GetNodeName(), *MoveTask->GetName());
				}
			}
			else
			{
				MyMemory->Task = MoveTask;
				UE_VLOG(MyController, LogBehaviorTree, Verbose, TEXT("\'%s\' task implementing move with task %s"), *GetNodeName(), *MoveTask->GetName());
				MoveTask->ReadyForActivation();
			}

			NodeResult = (MoveTask->GetState() != EGameplayTaskState::Finished) ? EBTNodeResult::InProgress :
				MoveTask->WasMoveSuccessful() ? EBTNodeResult::Succeeded :
				EBTNodeResult::Failed;
		}
	}

	return NodeResult;
}

UAITask_DashToLeader* UBTTask_DashToLeader::PrepareMoveTask(UBehaviorTreeComponent& OwnerComp, UAITask_DashToLeader* ExistingTask)
{
	UAITask_DashToLeader* MoveTask = ExistingTask ? ExistingTask : NewBTAITask<UAITask_DashToLeader>(OwnerComp);
	if (MoveTask)
	{
		const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
			APawn* TargetActor = Cast<APawn>(KeyValue);
			if (TargetActor)
			{
				auto TargetPt = TargetActor->GetActorLocation() + (TargetActor->GetActorForwardVector() * 100.f);
				MoveTask->SetUp(MoveTask->GetAIController(), TargetPt);
			}
		}
		else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
		}
	}

	return MoveTask;
}

