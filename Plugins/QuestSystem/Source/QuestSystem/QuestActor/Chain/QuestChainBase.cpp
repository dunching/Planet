#include "QuestChainBase.h"

#include "Kismet/GameplayStatics.h"
#include "GameplayTasksComponent.h"
#include "StateTreeConditionBase.h"

#include "QuestSystemStateTreeComponent.h"
#include "LogWriter.h"
#include "STT_QuestBase.h"
#include "STT_QuestChainBase.h"
#include "Tools.h"


const FName PlayerController(
                             TEXT(
                                  "PlayerController"
                                 )
                            );

USTQuestChainComponentSchemaBase::USTQuestChainComponentSchemaBase()
{
	check(
	      ContextDataDescs.Num() == 1 && ContextDataDescs[0].Struct == AActor::StaticClass()
	     );

	ContextActorClass = FOwnerType::StaticClass();
	PlayerControllerClass = APlayerController::StaticClass();
}

bool USTQuestChainComponentSchemaBase::IsStructAllowed(
	const UScriptStruct* InScriptStruct
	) const
{
	return Super::IsStructAllowed(
	                              InScriptStruct
	                             ) ||
	       InScriptStruct->IsChildOf(
	                                 FSTT_QuestChainBase::StaticStruct()
	                                ) ||
	       InScriptStruct->IsChildOf(
	                                 FSTT_Guide_Record::StaticStruct()
	                                );
}

void USTQuestChainComponentSchemaBase::PostLoad()
{
	Super::PostLoad();

	ContextDataDescs[0].Struct = ContextActorClass.Get();
}

bool USTQuestChainComponentSchemaBase::SetContextRequirements(
	UBrainComponent& BrainComponent,
	FStateTreeExecutionContext& Context,
	bool bLogErrors
	)
{
	if (!Context.IsValid())
	{
		return false;
	}

	auto OwnerPtr = BrainComponent.GetOwner<FOwnerType>();
	if (OwnerPtr)
	{
		auto PlayerControllerPtr = GEngine->GetFirstLocalPlayerController(GetWorldImp());

		Context.SetContextDataByName(
		                             PlayerController,
		                             FStateTreeDataView(
		                                                PlayerControllerPtr
		                                               )
		                            );
	}

	return Super::SetContextRequirements(
	                                     BrainComponent,
	                                     Context,
	                                     bLogErrors
	                                    );
}

TSubclassOf<UStateTreeSchema> UGuideThreadSystemStateTreeComponentBase::GetSchema() const
{
	return FSchemaType::StaticClass();
}

bool UGuideThreadSystemStateTreeComponentBase::SetContextRequirements(
	FStateTreeExecutionContext& Context,
	bool bLogErrors
	)
{
	Context.SetCollectExternalDataCallback(
	                                       FOnCollectStateTreeExternalData::CreateUObject(
		                                        this,
		                                        &ThisClass::CollectExternalData
		                                       )
	                                      );
	return FSchemaType::SetContextRequirements(
	                                           *this,
	                                           Context,
	                                           bLogErrors
	                                          );
}

AQuestChain_MainBase::AQuestChain_MainBase(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(
	       ObjectInitializer
	      )
{
}

EQuestChainType AQuestChain_MainBase::GetGuideThreadType() const
{
	return EQuestChainType::kMain;
}

EQuestChainType AQuestChain_BranchBase::GetGuideThreadType() const
{
	return EQuestChainType::kBrand;
}

AQuestChain_AreaBase::AQuestChain_AreaBase(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(
	       ObjectInitializer
	      )
{
}

inline EQuestChainType AQuestChain_AreaBase::GetGuideThreadType() const
{
	return EQuestChainType::kArea;
}

AQuestChainBase::AQuestChainBase(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(
	       ObjectInitializer
	      )
{
}

void AQuestChainBase::UpdateCurrentTaskNode(
	const TSoftObjectPtr<UPAD_TaskNode_Guide>& InTaskNode
	)
{
	// TaskNodeRef = InTaskNode;
	// OnCurrentTaskNodeChanged.Broadcast(TaskNodeRef);
}

void AQuestChainBase::UpdateCurrentTaskNode(
	const FTaskNodeDescript& TaskNodeDescript
	)
{
	CurrentTaskNodeDescript = TaskNodeDescript;
	OnCurrentTaskNodeChanged.Broadcast(
	                                   CurrentTaskNodeDescript
	                                  );
}

FTaskNodeDescript AQuestChainBase::GetCurrentTaskNodeDescript() const
{
	return CurrentTaskNodeDescript;
}

void AQuestChainBase::AddEvent(
	const FTaskNodeResuleHelper& TaskNodeResuleHelper
	)
{
	EventsSet.Add(
	              TaskNodeResuleHelper.TaskID,
	              TaskNodeResuleHelper
	             );
}

FTaskNodeResuleHelper AQuestChainBase::ConsumeEvent(
	const FGuid& InGuid
	)
{
	FTaskNodeResuleHelper Result;
	if (EventsSet.Contains(
	                       InGuid
	                      ))
	{
		Result = EventsSet[InGuid];
		EventsSet.Remove(
		                 InGuid
		                );
	}
	return Result;
}

FString AQuestChainBase::GetGuideThreadTitle() const
{
	return TaskName;
}
