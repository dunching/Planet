#include "STT_QuestChain.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"
#include "Kismet/KismetMathLibrary.h"

#include "HumanCharacter_Player.h"
#include "QuestsActorBase.h"
#include "GameplayTask_Quest.h"
#include "QuestChain.h"
#include "QuestChainGameplayTask.h"
#include "InventoryComponent.h"
#include "HumanCharacter_AI.h"
#include "OpenWorldSystem.h"
#include "PlanetPlayerController.h"
#include "AreaVolume.h"
#include "AssetRefMap.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterAttributesComponent.h"
#include "GameOptions.h"
#include "InputProcessorSubSystem_Imp.h"
#include "MainHUDLayout.h"
#include "OpenWorldDataLayer.h"
#include "PlanetChildActorComponent.h"
#include "PlanetRichTextBlock.h"
#include "SceneActor.h"
#include "STE_GuideThread.h"
#include "TargetPoint_Runtime.h"
#include "Tools.h"
#include "UIManagerSubSystem.h"
#include "MyMediaPlayer.h"
#include "TutorialMediaPlayer.h"

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

	auto PCPtr = Cast<APlanetPlayerController>(GEngine->GetFirstLocalPlayerController(GetWorldImp()));
	if (PCPtr)
	{
		for (const auto& Iter : InstanceData.RewardProxysMap)
		{
			PCPtr->AddProxy(Iter.Key, Iter.Value);
		}
	}

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
	                                                                       InstanceData.GuideThreadActorPtr->
	                                                                       FindComponentByInterface(
		                                                                        UGameplayTaskOwnerInterface::StaticClass()
		                                                                       )
	                                                                      );
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideThreadActorPtr;
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

	if (InstanceData.GuideThreadActorPtr)
	{
		InstanceData.GuideThreadActorPtr->UpdateCurrentTaskNode(
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
	if (!InstanceData.GuideThreadActorPtr)
	{
		checkNoEntry();
	}

	const auto TaskNodeResuleHelper = InstanceData.GuideThreadActorPtr->ConsumeEvent(
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
	                                                                       InstanceData.GuideThreadActorPtr->
	                                                                       FindComponentByInterface(
		                                                                        UGameplayTaskOwnerInterface::StaticClass()
		                                                                       )
	                                                                      );
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideThreadActorPtr;
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
		InstanceData.GameplayTaskPtr->SetGuideActor(
		                                            InstanceData.GuideThreadActorPtr
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
	                                                                       InstanceData.GuideThreadActorPtr->
	                                                                       FindComponentByInterface(
		                                                                        UGameplayTaskOwnerInterface::StaticClass()
		                                                                       )
	                                                                      );
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideThreadActorPtr;
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
		InstanceData.GameplayTaskPtr->SetGuideActor(
		                                            InstanceData.GuideThreadActorPtr
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

			if (InstanceData.bRunForever)
			{
				return EStateTreeRunStatus::Running;
			}
			return EStateTreeRunStatus::Succeeded;
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

	if (InstanceData.bRunForever)
	{
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
	return FSTT_QuestChainBase::GetTaskNodeDescripton(
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
		                                                                       InstanceData.GuideThreadActorPtr->
		                                                                       FindComponentByInterface(
			                                                                        UGameplayTaskOwnerInterface::StaticClass()
			                                                                       )
		                                                                      );
		if (!InstanceData.TaskOwner)
		{
			InstanceData.TaskOwner = InstanceData.GuideThreadActorPtr;
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

	if (InstanceData.GuideThreadActorPtr)
	{
		InstanceData.GuideThreadActorPtr->UpdateCurrentTaskNode(
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
	                                                                       InstanceData.GuideThreadActorPtr->
	                                                                       FindComponentByInterface(
		                                                                        UGameplayTaskOwnerInterface::StaticClass()
		                                                                       )
	                                                                      );
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideThreadActorPtr;
	}

	InstanceData.GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_WaitPlayerEquipment>(
		 *InstanceData.TaskOwner
		);

	if (InstanceData.GameplayTaskPtr)
	{
		InstanceData.GameplayTaskPtr->SetPlayerCharacter(
		                                                 InstanceData.PlayerCharacterPtr
		                                                );

		InstanceData.GameplayTaskPtr->bPlayerAssign = InstanceData.bPlayerAssign;
		InstanceData.GameplayTaskPtr->WeaponSocket = InstanceData.WeaponSocket;
		InstanceData.GameplayTaskPtr->SkillSocket = InstanceData.SkillSocket;
		InstanceData.GameplayTaskPtr->bIsEquipentCharacter = InstanceData.bIsEquipentCharacter;

		InstanceData.GameplayTaskPtr->ReadyForActivation();
	}

	if (InstanceData.GuideThreadActorPtr)
	{
		InstanceData.GuideThreadActorPtr->UpdateCurrentTaskNode(
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
	FSTT_QuestChainBase::Tick(
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

	FSTT_QuestChainBase::ExitState(
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

EStateTreeRunStatus FSTT_GuideThread_SpawnNPC::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	Super::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );
	auto PCPtr = Cast<
		APlanetPlayerController>(UGameplayStatics::GetPlayerController(InstanceData.GuideThreadActorPtr, 0));
	if (PCPtr)
	{
		InstanceData.NPC_ID = FGuid::NewGuid();

		FTransform Transform = FTransform::Identity;
		Transform.SetLocation(
		                      InstanceData.PlayerCharacterPtr->GetActorLocation() + (InstanceData.PlayerCharacterPtr->
			                      GetActorForwardVector() * 100)
		                     );
		Transform.SetRotation(
		                      UKismetMathLibrary::MakeRotFromZX(
		                                                        FVector::UpVector,
		                                                        -PCPtr->GetPawn()->GetActorForwardVector()
		                                                       ).Quaternion()
		                     );

		PCPtr->ServerSpawnCharacter(
		                            InstanceData.NPCClass,
		                            InstanceData.NPC_ID,
		                            Transform,
		                            InstanceData.TeammateOption
		                           );

		InstanceData.GloabVariable_Main->SpwanedCharacterAry.Empty();
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_GuideThread_SpawnNPC::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	// 获取对应的Characers
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(
	                                      InstanceData.GuideThreadActorPtr,
	                                      AHumanCharacter_AI::StaticClass(),
	                                      OutActors
	                                     );
	for (auto Iter : OutActors)
	{
		auto CharacterPtr = Cast<AHumanCharacter_AI>(Iter);
		if (
			CharacterPtr &&
			InstanceData.NPC_ID == CharacterPtr->GetCharacterAttributesComponent()->GetCharacterID()
		)
		{
			InstanceData.GloabVariable_Main->SpwanedCharacterAry.Add(CharacterPtr);
			InstanceData.GloabVariable_Main->TemporaryActorAry.Add(CharacterPtr);
			return EStateTreeRunStatus::Succeeded;
		}
	}

	return Super::Tick(Context, DeltaTime);
}

FTaskNodeDescript FSTT_GuideThread_SpawnNPC::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
	) const
{
	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.bIsFreshPreviouDescription = false;

	return TaskNodeDescript;
}

EStateTreeRunStatus FSTT_GuideThread_AttckCharacter::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	Super::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );

	if (InstanceData.GloabVariable_Main->SpwanedCharacterAry.IsEmpty())
	{
		return EStateTreeRunStatus::Failed;
	}
	else
	{
		InstanceData.HumanCharacterAI = InstanceData.GloabVariable_Main->SpwanedCharacterAry[0].Get();
	}

	InstanceData.TaskOwner = TScriptInterface<IGameplayTaskOwnerInterface>(
	                                                                       InstanceData.GuideThreadActorPtr->
	                                                                       FindComponentByInterface(
		                                                                        UGameplayTaskOwnerInterface::StaticClass()
		                                                                       )
	                                                                      );
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideThreadActorPtr;
	}

	InstanceData.GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_Guide_AttckCharacter>(
		 *InstanceData.TaskOwner
		);
	InstanceData.GameplayTaskPtr->SetPlayerCharacter(InstanceData.PlayerCharacterPtr);
	InstanceData.GameplayTaskPtr->HumanCharacterAI = InstanceData.HumanCharacterAI;
	InstanceData.GameplayTaskPtr->bIsKill = InstanceData.bIsKill;

	InstanceData.GameplayTaskPtr->ReadyForActivation();

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_GuideThread_AttckCharacter::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
	) const
{
	Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );

	if (InstanceData.bIsNotFreshed && InstanceData.HumanCharacterAI && InstanceData.HumanCharacterAI->
	    GetGroupManagger())
	{
		InstanceData.bIsNotFreshed = false;
		InstanceData.GuideThreadActorPtr->UpdateCurrentTaskNode(
		                                                        GetTaskNodeDescripton(
			                                                         Context
			                                                        )
		                                                       );
	}

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

void FSTT_GuideThread_AttckCharacter::ExitState(
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

	InstanceData.GloabVariable_Main->SpwanedCharacterAry.Empty();
	if (InstanceData.HumanCharacterAI)
	{
		for (int32 Index = 0; Index < InstanceData.GloabVariable_Main->TemporaryActorAry.Num(); Index--)
		{
			if (InstanceData.GloabVariable_Main->TemporaryActorAry[Index] == InstanceData.HumanCharacterAI)
			{
				InstanceData.GloabVariable_Main->TemporaryActorAry.RemoveAt(Index);
				break;
			}
		}

		auto PCPtr = Cast<
			APlanetPlayerController>(UGameplayStatics::GetPlayerController(InstanceData.GuideThreadActorPtr, 0));
		if (PCPtr)
		{
			PCPtr->ServerDestroyActor(InstanceData.HumanCharacterAI);
		}
	}

	Super::ExitState(
	                 Context,
	                 Transition
	                );
}

FTaskNodeDescript FSTT_GuideThread_AttckCharacter::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );

	if (InstanceData.HumanCharacterAI)
	{
		FTaskNodeDescript TaskNodeDescript;

		TaskNodeDescript.Description = FString::Printf(
		                                               TEXT(
		                                                    "Attack <%s>%s</>"
		                                                   ),
		                                               *RichText_Emphasis,
		                                               *InstanceData.HumanCharacterAI->GetCharacterProxy()->
		                                                             GetDisplayTitle()
		                                              );

		return TaskNodeDescript;
	}
	else
	{
		FTaskNodeDescript TaskNodeDescript;

		TaskNodeDescript.bIsFreshPreviouDescription = false;

		return TaskNodeDescript;
	}
}

void FSTID_GuideThread_OpenTutorialVideo::OpenTutorialVideo()
{
	auto CurrentLayoutPtr = UUIManagerSubSystem::GetInstance()->GetCurrentLayout();
	if (CurrentLayoutPtr)
	{
		CurrentLayoutPtr->DisplayWidget(
		                                GuideVideoWidgetClass,
		                                [this](
		                                auto WidgetPtr
		                                )
		                                {
			                                auto CurUIPtr = Cast<UTutorialMediaPlayer>(WidgetPtr);
			                                if (CurUIPtr)
			                                {
				                                CurUIPtr->FileMediaSourceRef = FileMediaSourceRef;
				                                CurUIPtr->SetOnClicked(std::bind(&FSTID_GuideThread_OpenTutorialVideo::OnConfirm, this));
				                                CurUIPtr->Display();
			                                }
		                                }
		                               );
	}
}

void FSTID_GuideThread_OpenTutorialVideo::OnConfirm()
{
	bIsConfirmed = true;
}

EStateTreeRunStatus FSTT_GuideThread_OpenTutorialVideo::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	Super::EnterState(Context, Transition);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
															  *this
															 );

	InstanceData.OpenTutorialVideo();

	UInputProcessorSubSystem_Imp::GetInstance()->SwitchShowCursor(true);
	
	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_GuideThread_OpenTutorialVideo::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
	) const
{
	Super::Tick(Context, DeltaTime);

	FInstanceDataType& InstanceData = Context.GetInstanceData(
															  *this
															 );

	if (InstanceData.bIsConfirmed)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return EStateTreeRunStatus::Running;
}

void FSTT_GuideThread_OpenTutorialVideo::ExitState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	UInputProcessorSubSystem_Imp::GetInstance()->SwitchShowCursor(false);
	
	Super::ExitState(Context, Transition);
}

FTaskNodeDescript FSTT_GuideThread_ToBeContinued::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
	) const
{
	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.Description = TEXT("未完待续...");

	return TaskNodeDescript;
}

EStateTreeRunStatus FSTT_GuideThread_Completet::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );
	InstanceData.GuideThreadActorPtr->bIsComleted = true;

	return EStateTreeRunStatus::Succeeded;
}

EStateTreeRunStatus FSTT_DelayTask::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );

	InstanceData.RemainTime = InstanceData.DelayTime;

	return Super::EnterState(Context, Transition);
}

EStateTreeRunStatus FSTT_DelayTask::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );

	InstanceData.RemainTime -= DeltaTime;
	if (InstanceData.RemainTime <= 0)
	{
		return EStateTreeRunStatus::Succeeded;
	}

	if (InstanceData.GuideThreadActorPtr)
	{
		InstanceData.GuideThreadActorPtr->UpdateCurrentTaskNode(
		                                                        GetTaskNodeDescripton(
			                                                         Context
			                                                        )
		                                                       );
	}
	return Super::Tick(Context, DeltaTime);
}

FTaskNodeDescript FSTT_DelayTask::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );

	auto GameOptionsPtr = UGameOptions::GetInstance();

	FTaskNodeDescript TaskNodeDescript;

	const auto RemainTimeStr = FString::Printf(TEXT("%.0lf"), InstanceData.RemainTime);
	TaskNodeDescript.Description = InstanceData.Descrption.Replace(TEXT("[Time]"), *RemainTimeStr);

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

EStateTreeRunStatus FSTT_QuestChain::EnterState(
	FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );

	// 因为这个PlayerCharacter可能会被销毁，所以我们在每次任务时重新获取而不是在STE里面获取
	InstanceData.PlayerCharacterPtr = Cast<AHumanCharacter_Player>(
	                                                               UGameplayStatics::GetPlayerCharacter(
		                                                                InstanceData.GuideThreadActorPtr,
		                                                                0
		                                                               )
	                                                              );

	return Super::EnterState(
	                         Context,
	                         Transition
	                        );
}

FTaskNodeDescript FSTT_QuestChain::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
	) const
{
	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.bIsFreshPreviouDescription = false;

	return TaskNodeDescript;
}

EStateTreeRunStatus FSTT_QuestChain_PressKey::EnterState(
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

	if (InstanceData.GuideThreadActorPtr)
	{
		InstanceData.GuideThreadActorPtr->UpdateCurrentTaskNode(
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

EStateTreeRunStatus FSTT_QuestChain_PressKey::Tick(
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

FTaskNodeDescript FSTT_QuestChain_PressKey::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );

	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.Description = InstanceData.Description.Replace(TEXT("{Key}"), *InstanceData.Key.ToString());

	return TaskNodeDescript;
}

EStateTreeRunStatus FSTT_GuideThread_OpenLayout::EnterState(
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
	                                                                       InstanceData.GuideThreadActorPtr->
	                                                                       FindComponentByInterface(
		                                                                        UGameplayTaskOwnerInterface::StaticClass()
		                                                                       )
	                                                                      );
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideThreadActorPtr;
	}

	InstanceData.GameplayTaskPtr = UGameplayTask::NewTask<UGameplayTask_WaitOpenLayout>(
		 *InstanceData.TaskOwner
		);

	if (InstanceData.GameplayTaskPtr)
	{
		InstanceData.GameplayTaskPtr->SetPlayerCharacter(
		                                                 InstanceData.PlayerCharacterPtr
		                                                );

		InstanceData.GameplayTaskPtr->TargetLayoutCommon = InstanceData.LayoutCommon;
		InstanceData.GameplayTaskPtr->TargetMenuType = InstanceData.MenuType;
		InstanceData.GameplayTaskPtr->PCPtr = Cast<APlanetPlayerController>(
		                                                                    UGameplayStatics::GetPlayerController(
			                                                                     InstanceData.PlayerCharacterPtr,
			                                                                     0
			                                                                    )
		                                                                   );

		InstanceData.GameplayTaskPtr->ReadyForActivation();
	}

	if (InstanceData.GuideThreadActorPtr)
	{
		InstanceData.GuideThreadActorPtr->UpdateCurrentTaskNode(
		                                                        GetTaskNodeDescripton(
			                                                         Context
			                                                        )
		                                                       );
	}

	return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_GuideThread_OpenLayout::Tick(
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

FTaskNodeDescript FSTT_GuideThread_OpenLayout::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );

	auto GameOptionsPtr = UGameOptions::GetInstance();

	FTaskNodeDescript TaskNodeDescript;

	switch (InstanceData.LayoutCommon)
	{
	case ELayoutCommon::kMenuLayout:
		{
			TaskNodeDescript.Description = InstanceData.Description.Replace(
			                                                                TEXT("{MenuLayout}"),
			                                                                *UUIManagerSubSystem::GetInstance()->
			                                                                GetMenuLayoutName(InstanceData.MenuType)
			                                                               );
		}
		break;
	default:
		{
			TaskNodeDescript.Description = InstanceData.Description.Replace(
			                                                                TEXT("{Layout}"),
			                                                                *UUIManagerSubSystem::GetInstance()->
			                                                                GetLayoutName(InstanceData.LayoutCommon)
			                                                               );
		};
	}

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
	                                                                       InstanceData.GuideThreadActorPtr->
	                                                                       FindComponentByInterface(
		                                                                        UGameplayTaskOwnerInterface::StaticClass()
		                                                                       )
	                                                                      );
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideThreadActorPtr;
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
	                                               *RichText_Key,
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
	                                                                       InstanceData.GuideThreadActorPtr->
	                                                                       FindComponentByInterface(
		                                                                        UGameplayTaskOwnerInterface::StaticClass()
		                                                                       )
	                                                                      );
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideThreadActorPtr;
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
	                                               *RichText_Key,
	                                               *GameOptionsPtr->RunKey.ToString()
	                                              );

	return TaskNodeDescript;
}

EStateTreeRunStatus FSTT_GuideThread_ShowCursor::EnterState(
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

	// 因为这个PlayerCharacter可能会被销毁，所以我们在每次任务时重新获取而不是在STE里面获取
	InstanceData.PCPtr = UGameplayStatics::GetPlayerController(
	                                                           InstanceData.PlayerCharacterPtr,
	                                                           0
	                                                          );

	auto GameOptionsPtr = UGameOptions::GetInstance();

	InstanceData.Key = GameOptionsPtr->ShowCursor;

	if (InstanceData.GuideThreadActorPtr)
	{
		InstanceData.GuideThreadActorPtr->UpdateCurrentTaskNode(
		                                                        GetTaskNodeDescripton(
			                                                         Context
			                                                        )
		                                                       );
	}

	if (InstanceData.PCPtr)
	{
		return EStateTreeRunStatus::Running;
	}

	return EStateTreeRunStatus::Failed;
}

EStateTreeRunStatus FSTT_GuideThread_ShowCursor::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );

	if (InstanceData.PCPtr->GetMouseCursor())
	{
		return EStateTreeRunStatus::Succeeded;
	}

	return Super::Tick(
	                   Context,
	                   DeltaTime
	                  );
}

FTaskNodeDescript FSTT_GuideThread_ShowCursor::GetTaskNodeDescripton(
	FStateTreeExecutionContext& Context
	) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(
	                                                          *this
	                                                         );

	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.Description = InstanceData.Description.Replace(TEXT("{Key}"), *InstanceData.Key.ToString());

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

	if (InstanceData.GuideThreadActorPtr)
	{
		InstanceData.GuideThreadActorPtr->UpdateCurrentTaskNode(
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
					                      InstanceData.TargetPoint_RuntimeClass,
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
					                      InstanceData.TargetPoint_RuntimeClass,
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
	                                                                       InstanceData.GuideThreadActorPtr->
	                                                                       FindComponentByInterface(
		                                                                        UGameplayTaskOwnerInterface::StaticClass()
		                                                                       )
	                                                                      );
	if (!InstanceData.TaskOwner)
	{
		InstanceData.TaskOwner = InstanceData.GuideThreadActorPtr;
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
		InstanceData.GameplayTaskPtr->TargetPoint_RuntimeClass = InstanceData.TargetPoint_RuntimeClass;

		InstanceData.GameplayTaskPtr->ReadyForActivation();
	}

	if (InstanceData.GuideThreadActorPtr)
	{
		InstanceData.GuideThreadActorPtr->UpdateCurrentTaskNode(
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
	FSTT_QuestChainBase::Tick(
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
