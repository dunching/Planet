#include "QuestChainGameplayTask.h"

#include "Kismet/GameplayStatics.h"

#include "AssetRefMap.h"
#include "CharacterAbilitySystemComponent.h"
#include "EventSubjectComponent.h"
#include "GameplayTagsLibrary.h"
#include "QuestsActorBase.h"
#include "QuestInteraction.h"
#include "QuestSubSystem.h"
#include "QuestChain.h"
#include "InventoryComponent.h"
#include "HumanCharacter_AI.h"
#include "HumanCharacter_Player.h"
#include "ItemProxy_Coin.h"
#include "ItemProxy_Consumable.h"
#include "MainHUD.h"
#include "MainHUDLayout.h"
#include "PlanetGameplayAbility.h"
#include "PlanetPlayerController.h"
#include "TargetPoint_Runtime.h"


class AMainHUD;

UGameplayTask_Guide::UGameplayTask_Guide(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Guide::SetGuideActor(
	TObjectPtr<AQuestChainBase> InGuideActorPtr
	)
{
	GuideActorPtr = InGuideActorPtr;
}

UGameplayTask_Guide_MoveToLocation::UGameplayTask_Guide_MoveToLocation(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Guide_MoveToLocation::Activate()
{
	Super::Activate();

	if (PlayerCharacterPtr)
	{
		FTransform AbsoluteTransform = FTransform::Identity;
		AbsoluteTransform.SetLocation(TargetLocation);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = GetOwnerActor();

		TargetPointPtr = PlayerCharacterPtr->GetWorld()->SpawnActor<ATargetPoint_Runtime>(
			 TargetPoint_RuntimeClass,
			 AbsoluteTransform,
			 SpawnParameters
			);
	}
}

void UGameplayTask_Guide_MoveToLocation::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);

	if (FVector::Distance(TargetLocation, PlayerCharacterPtr->GetActorLocation()) < ReachedRadius)
	{
		StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
		EndTask();
	}
}

void UGameplayTask_Guide_MoveToLocation::OnDestroy(
	bool bInOwnerFinished
	)
{
	if (TargetPointPtr)
	{
		TargetPointPtr->Destroy();
		TargetPointPtr = nullptr;
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_Guide_MoveToLocation::SetUp(
	const FVector& InTargetLocation,
	int32 InReachedRadius
	)
{
	TargetLocation = InTargetLocation;
	ReachedRadius = InReachedRadius;
}

UGameplayTask_Guide_WaitInputKey::UGameplayTask_Guide_WaitInputKey(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Guide_WaitInputKey::Activate()
{
	Super::Activate();

	PCPtr = UGameplayStatics::GetPlayerController(this, 0);
	if (PCPtr)
	{
	}
}

void UGameplayTask_Guide_WaitInputKey::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);

	if (PCPtr->IsInputKeyDown(Key))
	{
		StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
		EndTask();
	}
}

UGameplayTask_Guide_Monologue::UGameplayTask_Guide_Monologue(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Guide_Monologue::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UGameplayTask_Guide_Monologue::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);

	RemainingTime -= DeltaTime;

	if (RemainingTime <= 0.f)
	{
		if (SentenceIndex < ConversationsAry.Num())
		{
			ConditionalPerformTask();
		}
		else
		{
			StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
			EndTask();
		}
	}
}

void UGameplayTask_Guide_Monologue::OnDestroy(
	bool bInOwnerFinished
	)
{
	if (PlayerCharacterPtr)
	{
		PlayerCharacterPtr->GetConversationComponent()->CloseConversationborder();
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_Guide_Monologue::SetUp(
	const TArray<FTaskNode_Conversation_SentenceInfo>& InConversationsAry
	)
{
	ConversationsAry = InConversationsAry;
}

void UGameplayTask_Guide_Monologue::ConditionalPerformTask()
{
	if (ConversationsAry.IsValidIndex(SentenceIndex))
	{
		const auto& Ref = ConversationsAry[SentenceIndex];

		RemainingTime = Ref.DelayTime;

		PlayerCharacterPtr->GetConversationComponent()->DisplaySentence(Ref);
	}

	SentenceIndex++;
}

UGameplayTask_Guide_AddToTarget::UGameplayTask_Guide_AddToTarget(
	const FObjectInitializer& ObjectInitializer
	) :
	  Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsPausable = true;
}

void UGameplayTask_Guide_AddToTarget::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UGameplayTask_Guide_AddToTarget::SetUp(
	const TSubclassOf<AQuestInteractionBase>& InGuideInteractionActorClass,
	const TSoftObjectPtr<AHumanCharacter_AI>& InTargetCharacterPtr
	)
{
	GuideInteractionActorClass = InGuideInteractionActorClass;
	TargetCharacterPtr = InTargetCharacterPtr;
}

void UGameplayTask_Guide_AddToTarget::ConditionalPerformTask()
{
	if (
		TargetCharacterPtr.IsValid() &&
		GuideInteractionActorClass.Get()
	)
	{
		StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
		EndTask();
	}
}

void UGameplayTask_Guide_ConversationWithTarget::Activate()
{
	Super::Activate();

	ConditionalPerformTask();
}

void UGameplayTask_Guide_ConversationWithTarget::OnDestroy(
	bool bInOwnerFinished
	)
{
	if (TargetPointPtr)
	{
		TargetPointPtr->Destroy();
		TargetPointPtr = nullptr;
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_Guide_ConversationWithTarget::SetUp(
	const TSoftObjectPtr<AHumanCharacter_AI>& InTargetCharacterPtr
	)
{
	TargetCharacterPtr = InTargetCharacterPtr;
}

void UGameplayTask_Guide_ConversationWithTarget::ConditionalPerformTask()
{
	if (
		TargetCharacterPtr.IsValid()
	)
	{
		if (PlayerCharacterPtr)
		{
			FTransform AbsoluteTransform = FTransform::Identity;

			FActorSpawnParameters SpawnParameters;
			SpawnParameters.Owner = GetOwnerActor();

			TargetPointPtr = PlayerCharacterPtr->GetWorld()->SpawnActor<ATargetPoint_Runtime>(
				 TargetPoint_RuntimeClass,
				 AbsoluteTransform,
				 SpawnParameters
				);

			TargetPointPtr->AttachToActor(
			                              TargetCharacterPtr.Get(),
			                              FAttachmentTransformRules::KeepRelativeTransform
			                             );
		}
	}
}

void UGameplayTask_Guide_WaitComplete::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);

	if (GuideActorPtr)
	{
		const auto Result = GuideActorPtr->ConsumeEvent(TaskID);
		if (Result.GetIsValid())
		{
			TaskNodeResuleHelper = Result;

			StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
			EndTask();
		}
	}
}

void UGameplayTask_Guide_WaitComplete::SetUp(
	const FGuid& InTaskID
	)
{
	TaskID = InTaskID;
}

void UGameplayTask_Guide_CollectResource::Activate()
{
	Super::Activate();
	{
		auto ModifyItemProxyStrategySPtr = PlayerCharacterPtr->GetInventoryComponent()->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_Consumable>();
		if (ModifyItemProxyStrategySPtr)
		{
			OnConsumableProxyChangedHandle = ModifyItemProxyStrategySPtr->OnConsumableProxyChanged.AddCallback(
				 std::bind(&ThisClass::OnGetConsumableProxy, this, std::placeholders::_1, std::placeholders::_2)
				);
		}
	}
	{
		auto ModifyItemProxyStrategySPtr = PlayerCharacterPtr->GetInventoryComponent()->GetModifyItemProxyStrategy<
			FModifyItemProxyStrategy_Coin>();
		if (ModifyItemProxyStrategySPtr)
		{
			OnCoinProxyChangedHandle = ModifyItemProxyStrategySPtr->OnCoinProxyChanged.AddCallback(
				 std::bind(
				           &ThisClass::OnCoinProxyChanged,
				           this,
				           std::placeholders::_1,
				           std::placeholders::_2,
				           std::placeholders::_3
				          )
				);
		}
	}
}

void UGameplayTask_Guide_CollectResource::OnDestroy(
	bool bInOwnerFinished
	)
{
	if (OnConsumableProxyChangedHandle)
	{
		OnConsumableProxyChangedHandle.Reset();
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_Guide_CollectResource::SetUp(
	const FGameplayTag& InResourceType,
	int32 InNum
	)
{
	ResourceType = InResourceType;
	Num = InNum;
}

FTaskNodeDescript UGameplayTask_Guide_CollectResource::GetTaskNodeDescripton() const
{
	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.bIsFreshPreviouDescription = true;

	TaskNodeDescript.Description = FString::Printf(TEXT("采集(%d/%d)个%s"), CurrentNum, Num, *ResourceType.ToString());

	return TaskNodeDescript;
}

void UGameplayTask_Guide_CollectResource::OnGetConsumableProxy(
	const TSharedPtr<FConsumableProxy>& ConsumableProxySPtr,
	EProxyModifyType ProxyModifyType
	)
{
	if (ConsumableProxySPtr && ConsumableProxySPtr->GetProxyType().MatchesTag(ResourceType))
	{
		switch (ProxyModifyType)
		{
		case EProxyModifyType::kAdd:
			{
				CurrentNum += ConsumableProxySPtr->GetNum();
				UpdateDescription();
				if (CurrentNum >= Num)
				{
					StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
					EndTask();
					return;
				}
			}
			break;
		case EProxyModifyType::kPropertyChange:
			{
				CurrentNum += ConsumableProxySPtr->GetNum();
				UpdateDescription();
				if (CurrentNum >= Num)
				{
					StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
					EndTask();
					return;
				}
			}
			break;
		case EProxyModifyType::kRemove:
			break;
		default: ;
		}
	}
}

void UGameplayTask_Guide_CollectResource::OnCoinProxyChanged(
	const TSharedPtr<FCoinProxy>& ProxySPtr,
	EProxyModifyType ProxyModifyType,
	int32 Num_
	)
{
	if (ProxySPtr && ProxySPtr->GetProxyType().MatchesTag(ResourceType))
	{
		switch (ProxyModifyType)
		{
		case EProxyModifyType::kAdd:
			{
				CurrentNum += Num_;
				UpdateDescription();
				if (CurrentNum >= Num_)
				{
					StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
					EndTask();
					return;
				}
			}
			break;
		case EProxyModifyType::kPropertyChange:
			{
				CurrentNum += Num_;
				UpdateDescription();
				if (CurrentNum >= Num_)
				{
					StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
					EndTask();
					return;
				}
			}
			break;
		case EProxyModifyType::kRemove:
			break;
		default: ;
		}
	}
}

void UGameplayTask_Guide_CollectResource::UpdateDescription() const
{
	if (GuideActorPtr)
	{
		GuideActorPtr->UpdateCurrentTaskNode(GetTaskNodeDescripton());
	}
}

void UGameplayTask_Guide_DefeatEnemy::Activate()
{
	Super::Activate();

	DelegateHandle = PlayerCharacterPtr->GetCharacterAbilitySystemComponent()->
	                                     MakedDamageDelegate.AddCallback(
	                                                                     std::bind(
		                                                                      &ThisClass::OnActiveGEAddedDelegateToSelf,
		                                                                      this,
		                                                                      std::placeholders::_1
		                                                                     )
	                                                                    );
}

void UGameplayTask_Guide_DefeatEnemy::OnDestroy(
	bool bInOwnerFinished
	)
{
	if (DelegateHandle)
	{
		DelegateHandle.Reset();
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_Guide_DefeatEnemy::SetUp(
	const FGameplayTag& InEnemyType,
	int32 InNum
	)
{
	EnemyType = InEnemyType;
	Num = InNum;
}

FTaskNodeDescript UGameplayTask_Guide_DefeatEnemy::GetTaskNodeDescripton() const
{
	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.bIsFreshPreviouDescription = true;

	TaskNodeDescript.Description = FString::Printf(TEXT("击杀(%d/%d)个%s"), CurrentNum, Num, *EnemyType.ToString());

	return TaskNodeDescript;
}

void UGameplayTask_Guide_DefeatEnemy::OnActiveGEAddedDelegateToSelf(
	const FOnEffectedTargetCallback& ReceivedEventModifyDataCallback
	)
{
	if (
		ReceivedEventModifyDataCallback.TargetCharacterPtr &&
		ReceivedEventModifyDataCallback.TargetCharacterPtr->GetCharacterProxy()->GetProxyType().MatchesTag(EnemyType)
	)
	{
		if (ReceivedEventModifyDataCallback.bIsDeath)
		{
			CurrentNum++;
			UpdateDescription();
			if (CurrentNum >= Num)
			{
				StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
				EndTask();
			}
		}
	}
}

void UGameplayTask_Guide_DefeatEnemy::UpdateDescription() const
{
	if (GuideActorPtr)
	{
		GuideActorPtr->UpdateCurrentTaskNode(GetTaskNodeDescripton());
	}
}

void UGameplayTask_Guide_ReturnOpenWorld::TickTask(
	float DeltaTime
	)
{
	Super::TickTask(DeltaTime);

	TotalTime += DeltaTime;

	if (TotalTime > RemainTime)
	{
		StateTreeRunStatus = EStateTreeRunStatus::Succeeded;
		EndTask();
	}
}

void UGameplayTask_Guide_ReturnOpenWorld::SetUp(
	int32 RemainTime_
	)
{
	RemainTime = RemainTime_;
}

FTaskNodeDescript UGameplayTask_Guide_ReturnOpenWorld::GetTaskNodeDescripton() const
{
	FTaskNodeDescript TaskNodeDescript;

	TaskNodeDescript.bIsFreshPreviouDescription = true;

	const auto Time = RemainTime - TotalTime;
	TaskNodeDescript.Description = FString::Printf(TEXT("%.1lf后离开副本"), Time);

	return TaskNodeDescript;
}

void UGameplayTask_Guide_ActiveDash::Activate()
{
	Super::Activate();

	PlayerCharacterPtr->GetCharacterAbilitySystemComponent()->AbilityActivatedCallbacks.AddUObject(
		 this,
		 &ThisClass::FGenericAbilityDelegate
		);
}

void UGameplayTask_Guide_ActiveDash::OnDestroy(
	bool bInOwnerFinished
	)
{
	PlayerCharacterPtr->GetCharacterAbilitySystemComponent()->AbilityActivatedCallbacks.Remove(DelegateHandle);

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_Guide_ActiveDash::FGenericAbilityDelegate(
	UGameplayAbility* GAPtr

	)
{
	if (GAPtr)
	{
		if (GAPtr->IsA(GAClass))
		{
			EndTask();
		}
	}
}

void UGameplayTask_Guide_ActiveRun::Activate()
{
	Super::Activate();

	PlayerCharacterPtr->GetCharacterAbilitySystemComponent()->AbilityActivatedCallbacks.AddUObject(
		 this,
		 &ThisClass::FGenericAbilityDelegate
		);
}

void UGameplayTask_Guide_ActiveRun::OnDestroy(
	bool bInOwnerFinished
	)
{
	PlayerCharacterPtr->GetCharacterAbilitySystemComponent()->AbilityActivatedCallbacks.Remove(DelegateHandle);

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_Guide_ActiveRun::FGenericAbilityDelegate(
	UGameplayAbility* GAPtr

	)
{
	if (GAPtr)
	{
		if (GAPtr->IsA(GAClass))
		{
			EndTask();
		}
	}
}

void UGameplayTask_Guide_AttckCharacter::Activate()
{
	Super::Activate();

	if (HumanCharacterAI && PlayerCharacterPtr)
	{
		DelegateHandle = HumanCharacterAI->GetCharacterAbilitySystemComponent()->MakedDamageDelegate.AddCallback(
			 std::bind(&ThisClass::OnEffectOhterCharacter, this, std::placeholders::_1)
			);
	}
}

void UGameplayTask_Guide_AttckCharacter::OnDestroy(
	bool bInOwnerFinished
	)
{
	if (DelegateHandle)
	{
		DelegateHandle.Reset();
	}

	Super::OnDestroy(bInOwnerFinished);
}

void UGameplayTask_Guide_AttckCharacter::OnEffectOhterCharacter(
	const FOnEffectedTargetCallback& ReceivedEventModifyDataCallback

	)
{
	if (ReceivedEventModifyDataCallback.TargetCharacterPtr &&
	    ReceivedEventModifyDataCallback.TargetCharacterPtr == HumanCharacterAI &&
	    ReceivedEventModifyDataCallback.Damage > 0
	)
	{
		if (bIsKill)
		{
			if (ReceivedEventModifyDataCallback.bIsDeath)
			{
				EndTask();
			}
		}
		else
		{
			EndTask();
		}
	}
}
