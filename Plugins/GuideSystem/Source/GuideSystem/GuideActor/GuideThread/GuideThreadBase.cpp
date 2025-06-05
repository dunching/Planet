#include "GuideThreadBase.h"

#include "Kismet/GameplayStatics.h"
#include "GameplayTasksComponent.h"
#include "StateTreeConditionBase.h"

#include "GuideSystemStateTreeComponent.h"
#include "LogWriter.h"
#include "STT_GuideBase.h"
#include "STT_GuideThreadBase.h"
#include "Tools.h"


const FName PlayerController(
                             TEXT(
                                  "PlayerController"
                                 )
                            );

UStateTreeGuideThreadComponentSchemaBase::UStateTreeGuideThreadComponentSchemaBase()
{
	check(
	      ContextDataDescs.Num() == 1 && ContextDataDescs[0].Struct == AActor::StaticClass()
	     );

	ContextActorClass = FOwnerType::StaticClass();
	PlayerControllerClass = APlayerController::StaticClass();
}

bool UStateTreeGuideThreadComponentSchemaBase::IsStructAllowed(
	const UScriptStruct* InScriptStruct
	) const
{
	return Super::IsStructAllowed(
	                              InScriptStruct
	                             ) ||
	       InScriptStruct->IsChildOf(
	                                 FSTT_GuideThreadBase::StaticStruct()
	                                ) ||
	       InScriptStruct->IsChildOf(
	                                 FSTT_Guide_Record::StaticStruct()
	                                );
}

void UStateTreeGuideThreadComponentSchemaBase::PostLoad()
{
	Super::PostLoad();

	ContextDataDescs[0].Struct = ContextActorClass.Get();
}

bool UStateTreeGuideThreadComponentSchemaBase::SetContextRequirements(
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

AGuideThread_MainBase::AGuideThread_MainBase(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(
	       ObjectInitializer
	      )
{
}

EGuideThreadType AGuideThread_MainBase::GetGuideThreadType() const
{
	return EGuideThreadType::kMain;
}

EGuideThreadType AGuideThread_BranchBase::GetGuideThreadType() const
{
	return EGuideThreadType::kBrand;
}

AGuideThread_AreaBase::AGuideThread_AreaBase(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(
	       ObjectInitializer
	      )
{
}

inline EGuideThreadType AGuideThread_AreaBase::GetGuideThreadType() const
{
	return EGuideThreadType::kArea;
}

AGuideThreadBase::AGuideThreadBase(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(
	       ObjectInitializer
	      )
{
}

void AGuideThreadBase::UpdateCurrentTaskNode(
	const TSoftObjectPtr<UPAD_TaskNode_Guide>& InTaskNode
	)
{
	// TaskNodeRef = InTaskNode;
	// OnCurrentTaskNodeChanged.Broadcast(TaskNodeRef);
}

void AGuideThreadBase::UpdateCurrentTaskNode(
	const FTaskNodeDescript& TaskNodeDescript
	)
{
	CurrentTaskNodeDescript = TaskNodeDescript;
	OnCurrentTaskNodeChanged.Broadcast(
	                                   CurrentTaskNodeDescript
	                                  );
}

FTaskNodeDescript AGuideThreadBase::GetCurrentTaskNodeDescript() const
{
	return CurrentTaskNodeDescript;
}

void AGuideThreadBase::AddEvent(
	const FTaskNodeResuleHelper& TaskNodeResuleHelper
	)
{
	EventsSet.Add(
	              TaskNodeResuleHelper.TaskID,
	              TaskNodeResuleHelper
	             );
}

FTaskNodeResuleHelper AGuideThreadBase::ConsumeEvent(
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

FString AGuideThreadBase::GetGuideThreadTitle() const
{
	return TaskName;
}
