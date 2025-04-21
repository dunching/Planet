#include "STT_GuideThread.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"

#include "HumanCharacter_Player.h"
#include "GuideActor.h"
#include "GuideSystemGameplayTask.h"
#include "GuideThread.h"
#include "GuideThreadGameplayTask.h"
#include "InventoryComponent.h"
#include "HumanCharacter_AI.h"
#include "OpenWorldSystem.h"
#include "PlanetPlayerController.h"
#include "Planet_Tools.h"
#include "AreaVolume.h"
#include "AssetRefMap.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "GameOptions.h"
#include "OpenWorldDataLayer.h"
#include "PlanetChildActorComponent.h"
#include "PlanetRichTextBlock.h"
#include "SceneActor.h"
#include "STE_GuideThread.h"
#include "TargetPoint_Runtime.h"

const UStruct* FSTT_GuideThreadRecord::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideThreadRecord::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (InstanceData.GuideActorPtr)
	{
		// 读取任务进度
		const auto PreviousTaskID = InstanceData.GuideActorPtr->GetPreviousTaskID();
		if (PreviousTaskID.IsValid())
		{
			if (PreviousTaskID == InstanceData.TaskID)
			{
				InstanceData.GuideActorPtr->SetPreviousTaskID(
					FGuid()
				);

				return Super::EnterState(
					Context,
					Transition
				);
			}

			return EStateTreeRunStatus::Succeeded;
		}
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}

	// 记录当前的任务ID，
	InstanceData.GuideActorPtr->SetCurrentTaskID(
		InstanceData.TaskID
	);

	return Super::EnterState(
		Context,
		Transition
	);
}

EStateTreeRunStatus FSTT_GuideThreadBase::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (InstanceData.GuideActorPtr)
	{
		InstanceData.GuideActorPtr->UpdateCurrentTaskNode(
			GetTaskNodeDescripton(
				Context
			)
		);

		// 因为这个PlayerCharacter可能会被销毁，所以我们在每次任务时重新获取而不是在STE里面获取
		InstanceData.PlayerCharacterPtr = Cast<AHumanCharacter_Player>(
			UGameplayStatics::GetPlayerCharacter(
				InstanceData.GuideActorPtr,
				0
			)
		);
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}

	return Super::EnterState(
		Context,
		Transition
	);
}

void FSTT_GuideThreadBase::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::ExitState(
		Context,
		Transition
	);
}

FTaskNodeDescript FSTT_GuideThreadBase::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.bIsFreshPreviouDescription = false;

	return TaskNodeDescript;
}

EStateTreeRunStatus FSTT_GuideThreadFail::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	// TODO 任务失败时 

	checkNoEntry();

	return Super::EnterState(
		Context,
		Transition
	);
}

const UStruct* FSTT_GuideThreadDistributeRewards::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideThreadDistributeRewards::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	// 
	Super::EnterState(
		Context,
		Transition
	);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	InstanceData.PlayerCharacterPtr->GetInventoryComponent()->AddProxys_Server(
		InstanceData.RewardsItemID
	);

	return EStateTreeRunStatus::Succeeded;
}

const UStruct* FSTT_GuideThreadMonologue::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideThreadMonologue::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(
		Context,
		Transition
	);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.GuideActorPtr->FindComponentByInterface(
			UGameplayTaskOwnerInterface::StaticClass()
		)
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideActorPtr;
	}

	// TODO -》spawn TaskNodeRef

	return PerformGameplayTask(
		Context
	);
}

EStateTreeRunStatus FSTT_GuideThreadMonologue::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	// Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (!InstanceData.GameplayTaskPtr)
	{
		checkNoEntry();
	}

	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(
		Context,
		DeltaTime
	);
}

void FSTT_GuideThreadMonologue::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.GameplayTaskPtr->ExternalCancel();
	}
	InstanceData.GameplayTaskPtr = nullptr;

	Super::ExitState(
		Context,
		Transition
	);
}

EStateTreeRunStatus FSTT_GuideThreadMonologue::PerformGameplayTask(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	auto GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Guide_Monologue>(
		*InstanceData.TaskOwner
	);

	if (GameplayTaskPtr)
	{
		GameplayTaskPtr->SetPlayerCharacter(
			InstanceData.PlayerCharacterPtr
		);
		GameplayTaskPtr->SetUp(
			InstanceData.ConversationsAry
		);
		GameplayTaskPtr->ReadyForActivation();

		InstanceData.GameplayTaskPtr = GameplayTaskPtr;
	}

	return EStateTreeRunStatus::Running;
}

FTaskNodeDescript FSTT_GuideThreadMonologue::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.bIsFreshPreviouDescription = false;

	return TaskNodeDescript;
}

EStateTreeRunStatus FSTT_GuideThread_WaitTaskComplete::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(
		Context,
		Transition
	);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (InstanceData.GuideActorPtr)
	{
		InstanceData.GuideActorPtr->UpdateCurrentTaskNode(
			GetTaskNodeDescripton(
				Context
			)
		);
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_GuideThread_WaitTaskComplete::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	// Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (!InstanceData.GuideActorPtr)
	{
		checkNoEntry();
	}

	const auto TaskNodeResuleHelper = InstanceData.GuideActorPtr->ConsumeEvent(
		InstanceData.NotifyTaskID
	);
	if (TaskNodeResuleHelper.GetIsValid())
	{
		InstanceData.TaskNodeResuleHelper = TaskNodeResuleHelper;
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(
		Context,
		DeltaTime
	);
}

FTaskNodeDescript FSTT_GuideThread_WaitTaskComplete::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.bIsFreshPreviouDescription = true;
	TaskNodeDescript.Description = InstanceData.PromtStr;

	return TaskNodeDescript;
}

const UStruct* FSTT_GuideThreadCollectResource::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideThreadCollectResource::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	// 因为获取任务描述需要用到GameplayTaskPtr，所以我们这里提前初始化这个Task
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.GuideActorPtr->FindComponentByInterface(
			UGameplayTaskOwnerInterface::StaticClass()
		)
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideActorPtr;
	}

	InstanceData.GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Guide_CollectResource>(
		*InstanceData.TaskOwner
	);
	InstanceData.GameplayTaskPtr->SetUp(
		InstanceData.ResourceType,
		InstanceData.Num
	);

	Super::EnterState(
		Context,
		Transition
	);

	// TODO -》spawn TaskNodeRef

	return PerformGameplayTask(
		Context
	);
}

EStateTreeRunStatus FSTT_GuideThreadCollectResource::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	// Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (!InstanceData.GameplayTaskPtr)
	{
		checkNoEntry();
	}

	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(
		Context,
		DeltaTime
	);
}

void FSTT_GuideThreadCollectResource::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.GameplayTaskPtr->ExternalCancel();
	}
	InstanceData.GameplayTaskPtr = nullptr;

	Super::ExitState(
		Context,
		Transition
	);
}

EStateTreeRunStatus FSTT_GuideThreadCollectResource::PerformGameplayTask(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (InstanceData.GameplayTaskPtr)
	{
		InstanceData.GameplayTaskPtr->SetPlayerCharacter(
			InstanceData.PlayerCharacterPtr
		);
		InstanceData.GameplayTaskPtr->SetTaskID(
			InstanceData.TaskID
		);
		InstanceData.GameplayTaskPtr->SetGuideActor(
			InstanceData.GuideActorPtr
		);

		InstanceData.GameplayTaskPtr->ReadyForActivation();
	}

	return EStateTreeRunStatus::Running;
}

FTaskNodeDescript FSTT_GuideThreadCollectResource::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	return InstanceData.GameplayTaskPtr->GetTaskNodeDescripton();
}

const UStruct* FSTT_GuideThreadDefeatEnemy::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideThreadDefeatEnemy::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	// 因为获取任务描述需要用到GameplayTaskPtr，所以我们这里提前初始化这个Task
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.GuideActorPtr->FindComponentByInterface(
			UGameplayTaskOwnerInterface::StaticClass()
		)
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideActorPtr;
	}

	InstanceData.GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Guide_DefeatEnemy>(
		*InstanceData.TaskOwner
	);
	InstanceData.GameplayTaskPtr->SetUp(
		InstanceData.EnemyType,
		InstanceData.Num
	);

	Super::EnterState(
		Context,
		Transition
	);

	// TODO -》spawn TaskNodeRef

	return PerformGameplayTask(
		Context
	);
}

EStateTreeRunStatus FSTT_GuideThreadDefeatEnemy::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	// Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (!InstanceData.GameplayTaskPtr)
	{
		checkNoEntry();
	}

	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(
		Context,
		DeltaTime
	);
}

void FSTT_GuideThreadDefeatEnemy::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.GameplayTaskPtr->ExternalCancel();
	}
	InstanceData.GameplayTaskPtr = nullptr;

	Super::ExitState(
		Context,
		Transition
	);
}

EStateTreeRunStatus FSTT_GuideThreadDefeatEnemy::PerformGameplayTask(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (InstanceData.GameplayTaskPtr)
	{
		InstanceData.GameplayTaskPtr->SetPlayerCharacter(
			InstanceData.PlayerCharacterPtr
		);
		InstanceData.GameplayTaskPtr->SetTaskID(
			InstanceData.TaskID
		);
		InstanceData.GameplayTaskPtr->SetGuideActor(
			InstanceData.GuideActorPtr
		);

		InstanceData.GameplayTaskPtr->ReadyForActivation();
	}

	return EStateTreeRunStatus::Running;
}

FTaskNodeDescript FSTT_GuideThreadDefeatEnemy::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	return InstanceData.GameplayTaskPtr->GetTaskNodeDescripton();
}

const UStruct* FSTT_GuideThread_ChangeNPCsInteractionList::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideThread_ChangeNPCsInteractionList::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
)
const
{
	Super::EnterState(
		Context,
		Transition
	);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (auto PAD = InstanceData.PAD.LoadSynchronous())
	{
		auto TargetCharacterPtr = PAD->TargetCharacterPtr.LoadSynchronous();
		if (TargetCharacterPtr)
		{
			TargetCharacterPtr->GetSceneActorInteractionComponent()->ChangedInterationState(
				InstanceData.GuideInteractionActorClass,
				InstanceData.bEnable
			);

			if (InstanceData.bIsInfinish)
			{
				return EStateTreeRunStatus::Succeeded;
			}

			return EStateTreeRunStatus::Running;
		}
	}

	return EStateTreeRunStatus::Failed;
}

void FSTT_GuideThread_ChangeNPCsInteractionList::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (auto PAD = InstanceData.PAD.LoadSynchronous())
	{
		auto TargetCharacterPtr = PAD->TargetCharacterPtr.LoadSynchronous();
		if (TargetCharacterPtr)
		{
			TargetCharacterPtr->GetSceneActorInteractionComponent()->ChangedInterationState(
				InstanceData.GuideInteractionActorClass,
				!InstanceData.bEnable
			);
		}
	}

	Super::ExitState(
		Context,
		Transition
	);
}

const UStruct* FSTT_GuideThreadLeaveHere::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

FTaskNodeDescript FSTT_GuideThreadLeaveHere::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.Description = InstanceData.Prompt.Sentence;

	return TaskNodeDescript;
}

const UStruct* FSTT_GuideThreadBase::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_GuideThreadLeaveHere::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(
		Context,
		Transition
	);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	InstanceData.PAD.LoadSynchronous();
	if (InstanceData.PAD->AreaVolumePtr)
	{
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FSTT_GuideThreadLeaveHere::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	// TODO Task里面用代理去做
	if (!InstanceData.PAD->AreaVolumePtr->IsOverlappingActor(
		InstanceData.PlayerCharacterPtr
	))
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(
		Context,
		DeltaTime
	);
}

EStateTreeRunStatus FSTGT_GuideThreadCheckIsInValidArea::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(
		Context,
		Transition
	);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	InstanceData.PAD.LoadSynchronous();
	if (InstanceData.PAD->AreaVolumePtr)
	{
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FSTGT_GuideThreadCheckIsInValidArea::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	InstanceData.Total += DeltaTime;
	if (InstanceData.Total > 1.f)
	{
		// TODO Task里面用代理去做
		if (!InstanceData.PAD->AreaVolumePtr->IsOverlappingActor(
			InstanceData.PlayerCharacterPtr
		))
		{
			return EStateTreeRunStatus::Stopped;
		}
	}

	return Super::Tick(
		Context,
		DeltaTime
	);
}

FTaskNodeDescript FSTGT_GuideThreadCheckIsInValidArea::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	return FSTT_GuideThreadBase::GetTaskNodeDescripton(
		Context
	);
}


EStateTreeRunStatus FSTT_GuideThreadReturnOpenWorld::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (InstanceData.RemainTime > 0)
	{
		InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
			InstanceData.GuideActorPtr->FindComponentByInterface(
				UGameplayTaskOwnerInterface::StaticClass()
			)
		);
		if (!InstanceData.TaskOwner)
		{
			InstanceData.TaskOwner = InstanceData.GuideActorPtr;
		}

		InstanceData.GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Guide_ReturnOpenWorld>(
			*InstanceData.TaskOwner
		);
		InstanceData.GameplayTaskPtr->SetUp(
			InstanceData.RemainTime
		);
		InstanceData.GameplayTaskPtr->ReadyForActivation();
	}

	return Super::EnterState(
		Context,
		Transition
	);
}

EStateTreeRunStatus FSTT_GuideThreadReturnOpenWorld::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (InstanceData.GuideActorPtr)
	{
		InstanceData.GuideActorPtr->UpdateCurrentTaskNode(
			GetTaskNodeDescripton(
				Context
			)
		);
	}

	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		auto PCPtr = Cast<APlanetPlayerController>(
			UGameplayStatics::GetPlayerController(
				GetWorldImp(),
				0
			)
		);
		if (PCPtr)
		{
			PCPtr->EntryChallengeLevel(
				ETeleport::kReturnOpenWorld
			);
		}

		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(
		Context,
		DeltaTime
	);
}

FTaskNodeDescript FSTT_GuideThreadReturnOpenWorld::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	return InstanceData.GameplayTaskPtr->GetTaskNodeDescripton();
}

EStateTreeRunStatus FSTT_GuideThread_WaitPlayerEquipment::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(
		Context,
		Transition
	);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.GuideActorPtr->FindComponentByInterface(
			UGameplayTaskOwnerInterface::StaticClass()
		)
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideActorPtr;
	}

	InstanceData.GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_WaitPlayerEquipment>(
		*InstanceData.TaskOwner
	);

	if (InstanceData.GameplayTaskPtr)
	{
		InstanceData.GameplayTaskPtr->SetPlayerCharacter(
			InstanceData.PlayerCharacterPtr
		);

		InstanceData.GameplayTaskPtr->WeaponSocket = InstanceData.WeaponSocket;
		InstanceData.GameplayTaskPtr->SkillSocket = InstanceData.SkillSocket;

		InstanceData.GameplayTaskPtr->ReadyForActivation();
	}

	if (InstanceData.GuideActorPtr)
	{
		InstanceData.GuideActorPtr->UpdateCurrentTaskNode(
			GetTaskNodeDescripton(
				Context
			)
		);
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_GuideThread_WaitPlayerEquipment::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FSTT_GuideThreadBase::Tick(
		Context,
		DeltaTime
	);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (!InstanceData.GameplayTaskPtr)
	{
		checkNoEntry();
	}

	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

void FSTT_GuideThread_WaitPlayerEquipment::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.GameplayTaskPtr->ExternalCancel();
	}
	InstanceData.GameplayTaskPtr = nullptr;

	FSTT_GuideThreadBase::ExitState(
		Context,
		Transition
	);
}

FTaskNodeDescript FSTT_GuideThread_WaitPlayerEquipment::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.Description = TEXT(
		"装备武器和技能"
	);

	return TaskNodeDescript;
}

void FSTT_GuideThreadReturnOpenWorld::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.GameplayTaskPtr->ExternalCancel();
	}
	InstanceData.GameplayTaskPtr = nullptr;

	Super::ExitState(
		Context,
		Transition
	);
}

EStateTreeRunStatus FSTT_GuideThread_PressKey::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(
		Context,
		Transition
	);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (InstanceData.GuideActorPtr)
	{
		InstanceData.GuideActorPtr->UpdateCurrentTaskNode(
			GetTaskNodeDescripton(
				Context
			)
		);
	}

	// 因为这个PlayerCharacter可能会被销毁，所以我们在每次任务时重新获取而不是在STE里面获取
	InstanceData.PCPtr = UGameplayStatics::GetPlayerController(
		InstanceData.PlayerCharacterPtr,
		0
	);
	if (InstanceData.PCPtr)
	{
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FSTT_GuideThread_PressKey::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (InstanceData.PCPtr->IsInputKeyDown(
		InstanceData.Key
	))
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(
		Context,
		DeltaTime
	);
}

FTaskNodeDescript FSTT_GuideThread_PressKey::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.Description = FString::Printf(
		TEXT(
			"Press <%s>%s</> key To Move"
		),
		*Rich_Key,
		*InstanceData.Key.ToString()
	);

	return TaskNodeDescript;
}

EStateTreeRunStatus FSTT_GuideThread_ActiveDash::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.GuideActorPtr->FindComponentByInterface(
			UGameplayTaskOwnerInterface::StaticClass()
		)
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideActorPtr;
	}

	InstanceData.GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Guide_ActiveDash>(
		*InstanceData.TaskOwner
	);
	InstanceData.GameplayTaskPtr->GAClass = InstanceData.GAClass;
	InstanceData.GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);

	InstanceData.GameplayTaskPtr->ReadyForActivation();

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_GuideThread_ActiveDash::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (!InstanceData.GameplayTaskPtr)
	{
		checkNoEntry();
	}

	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

void FSTT_GuideThread_ActiveDash::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.GameplayTaskPtr->ExternalCancel();
	}
	InstanceData.GameplayTaskPtr = nullptr;

	Super::ExitState(
		Context,
		Transition
	);
}

FTaskNodeDescript FSTT_GuideThread_ActiveDash::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	auto GameOptionsPtr = UGameOptions::GetInstance();

	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.Description = FString::Printf(
		TEXT(
			"Press <%s>%s</> key To <Button Display=\"Dash\" Param=\"Dash\"/>"
		),
		*Rich_Key,
		*GameOptionsPtr->DashKey.ToString()
	);

	return TaskNodeDescript;
}

EStateTreeRunStatus FSTT_GuideThread_Run::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.GuideActorPtr->FindComponentByInterface(
			UGameplayTaskOwnerInterface::StaticClass()
		)
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideActorPtr;
	}

	InstanceData.GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Guide_ActiveRun>(
		*InstanceData.TaskOwner
	);
	InstanceData.GameplayTaskPtr->GAClass = InstanceData.GAClass;
	InstanceData.GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);

	InstanceData.GameplayTaskPtr->ReadyForActivation();

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_GuideThread_Run::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (!InstanceData.GameplayTaskPtr)
	{
		checkNoEntry();
	}

	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

void FSTT_GuideThread_Run::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.GameplayTaskPtr->ExternalCancel();
	}
	InstanceData.GameplayTaskPtr = nullptr;

	Super::ExitState(
		Context,
		Transition
	);
}

FTaskNodeDescript FSTT_GuideThread_Run::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	auto GameOptionsPtr = UGameOptions::GetInstance();

	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.Description = FString::Printf(
		TEXT(
			"Press <%s>%s</> key To <Button Display=\"Run\" Param=\"Run\"/>"
		),
		*Rich_Key,
		*GameOptionsPtr->RunKey.ToString()
	);

	return TaskNodeDescript;
}

EStateTreeRunStatus FSTT_GuideThread_GoToTheTargetPoint::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(
		Context,
		Transition
	);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (InstanceData.GuideActorPtr)
	{
		InstanceData.GuideActorPtr->UpdateCurrentTaskNode(
			GetTaskNodeDescripton(
				Context
			)
		);
	}

	if (InstanceData.PlayerCharacterPtr)
	{
		auto PADPtr = InstanceData.PAD.LoadSynchronous();
		if (PADPtr)
		{
			if (auto TargetCharacterPtr = PADPtr->TargetCharacterPtr.LoadSynchronous())
			{
				InstanceData.TargetCharacterPtr = TargetCharacterPtr;
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = InstanceData.PlayerCharacterPtr;

				InstanceData.TargetPointPtr = InstanceData.PlayerCharacterPtr->GetWorld()->SpawnActor<
					ATargetPoint_Runtime>(
					UAssetRefMap::GetInstance()->TargetPoint_RuntimeClass,
					SpawnParameters
				);

				InstanceData.TargetPointPtr->AttachToActor(
					InstanceData.TargetCharacterPtr,
					FAttachmentTransformRules::KeepRelativeTransform
				);

				return EStateTreeRunStatus::Running;
			}
			else if (auto TargetPointPtr = PADPtr->TargetPointPtr.LoadSynchronous())
			{
				InstanceData.TargetLocation = TargetPointPtr->GetActorLocation();

				FTransform AbsoluteTransform = FTransform::Identity;
				AbsoluteTransform.SetLocation(
					InstanceData.TargetLocation
				);

				FActorSpawnParameters SpawnParameters;
				SpawnParameters.Owner = InstanceData.PlayerCharacterPtr;

				InstanceData.TargetPointPtr = InstanceData.PlayerCharacterPtr->GetWorld()->SpawnActor<
					ATargetPoint_Runtime>(
					UAssetRefMap::GetInstance()->TargetPoint_RuntimeClass,
					AbsoluteTransform,
					SpawnParameters
				);

				return EStateTreeRunStatus::Running;
			}
		}
	}

	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FSTT_GuideThread_GoToTheTargetPoint::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	if (InstanceData.TargetCharacterPtr)
	{
		const auto Distance = FVector::Distance(
			InstanceData.TargetCharacterPtr->GetActorLocation(),
			InstanceData.PlayerCharacterPtr->GetActorLocation()
		);
		if (Distance < InstanceData.ReachedRadius)
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}
	else
	{
		const auto Distance = FVector::Distance(
			InstanceData.TargetLocation,
			InstanceData.PlayerCharacterPtr->GetActorLocation()
		);
		if (Distance < InstanceData.ReachedRadius)
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}

	return Super::Tick(
		Context,
		DeltaTime
	);
}

FTaskNodeDescript FSTT_GuideThread_GoToTheTargetPoint::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	FTaskNodeDescript TaskNodeDescript;

	if (InstanceData.PromtStr.IsEmpty())
	{
		TaskNodeDescript.Description = TEXT(
			"移动至目标点"
		);
	}
	else
	{
		TaskNodeDescript.Description = InstanceData.PromtStr;
	}

	return TaskNodeDescript;
}

void FSTT_GuideThread_GoToTheTargetPoint::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (InstanceData.TargetPointPtr)
	{
		InstanceData.TargetPointPtr->Destroy();
	}
	InstanceData.TargetPointPtr = nullptr;

	Super::ExitState(
		Context,
		Transition
	);
}

EStateTreeRunStatus FSTT_GuideThread_WaitInteractionSceneActor::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::EnterState(
		Context,
		Transition
	);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
		InstanceData.GuideActorPtr->FindComponentByInterface(
			UGameplayTaskOwnerInterface::StaticClass()
		)
	);
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideActorPtr;
	}

	InstanceData.GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_WaitInteractionSceneActor>(
		*InstanceData.TaskOwner
	);

	if (InstanceData.GameplayTaskPtr)
	{
		InstanceData.GameplayTaskPtr->SetPlayerCharacter(
			InstanceData.PlayerCharacterPtr
		);

		InstanceData.GameplayTaskPtr->PAD = InstanceData.PAD;

		InstanceData.GameplayTaskPtr->ReadyForActivation();
	}

	if (InstanceData.GuideActorPtr)
	{
		InstanceData.GuideActorPtr->UpdateCurrentTaskNode(
			GetTaskNodeDescripton(
				Context
			)
		);
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_GuideThread_WaitInteractionSceneActor::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FSTT_GuideThreadBase::Tick(
		Context,
		DeltaTime
	);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (!InstanceData.GameplayTaskPtr)
	{
		checkNoEntry();
	}

	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() == EGameplayTaskState::Finished)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

void FSTT_GuideThread_WaitInteractionSceneActor::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
) const
{
	Super::ExitState(
		Context,
		Transition
	);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);
	if (InstanceData.GameplayTaskPtr && InstanceData.GameplayTaskPtr->GetState() != EGameplayTaskState::Finished)
	{
		InstanceData.GameplayTaskPtr->ExternalCancel();
	}
	InstanceData.GameplayTaskPtr = nullptr;
}

FTaskNodeDescript FSTT_GuideThread_WaitInteractionSceneActor::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
		*this
	);

	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.Description = TEXT(
		"前往与目标交互!"
	);

	return TaskNodeDescript;
}
