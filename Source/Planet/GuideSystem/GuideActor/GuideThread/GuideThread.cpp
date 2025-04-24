#include "GuideThread.h"

#include "Kismet/GameplayStatics.h"
#include "GameplayTasksComponent.h"
#include "StateTreeConditionBase.h"

#include "STT_GuideThread.h"

#include "GuideSystemStateTreeComponent.h"
#include "LogWriter.h"
#include "PlanetPlayerController.h"


bool FTaskNodeResuleHelper::GetIsValid() const
{
	return TaskID.IsValid();
}

const FName PlayerController(
	TEXT(
		"PlayerController"
	)
);

UStateTreeGuideThreadComponentSchema::UStateTreeGuideThreadComponentSchema()
{
	check(
		ContextDataDescs.Num() == 1 && ContextDataDescs[0].Struct == AActor::StaticClass()
	);

	ContextActorClass = FOwnerType::StaticClass();
	PlayerControllerClass = APlanetPlayerController::StaticClass();

	ContextDataDescs[0].Struct = ContextActorClass.Get();
	ContextDataDescs.Emplace(
		PlayerController,
		PlayerControllerClass.Get(),
		FGuid(
			0xEDB3CD90,
			0x95F94E0A,
			0xBD15207B,
			0x98645CDC
		)
	);
}

bool UStateTreeGuideThreadComponentSchema::IsStructAllowed(
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
			FSTT_GuideThreadRecord::StaticStruct()
		);
}

void UStateTreeGuideThreadComponentSchema::PostLoad()
{
	Super::PostLoad();
}

bool UStateTreeGuideThreadComponentSchema::SetContextRequirements(
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
		auto PlayerControllerPtr = Cast<APlanetPlayerController>(
			UGameplayStatics::GetPlayerController(
				OwnerPtr,
				0
			)
		);

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

TSubclassOf<UStateTreeSchema> UGuideThreadSystemStateTreeComponent::GetSchema() const
{
	return UStateTreeGuideThreadComponentSchema::StaticClass();
}

bool UGuideThreadSystemStateTreeComponent::SetContextRequirements(
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
	return UStateTreeGuideThreadComponentSchema::SetContextRequirements(
		*this,
		Context,
		bLogErrors
	);
}

AGuideThread_Main::AGuideThread_Main(
	const FObjectInitializer& ObjectInitializer
):
 Super(
	 ObjectInitializer.
	 SetDefaultSubobjectClass<
		 UGuideMainThreadSystemStateTreeComponent>(
		 UGuideMainThreadSystemStateTreeComponent::ComponentName
	 )
 )
{
}

EGuideThreadType AGuideThread_Main::GetGuideThreadType() const
{
	return EGuideThreadType::kMain;
}

UStateTreeGuideMainThreadComponentSchema::UStateTreeGuideMainThreadComponentSchema()
{
	ContextActorClass = FOwnerType::StaticClass();

	ContextDataDescs[0].Struct = ContextActorClass.Get();
}

TSubclassOf<UStateTreeSchema> UGuideMainThreadSystemStateTreeComponent::GetSchema() const
{
	return FSchemaType::StaticClass();
}

bool UGuideMainThreadSystemStateTreeComponent::SetContextRequirements(
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

EGuideThreadType AGuideThread_Branch::GetGuideThreadType() const
{
	return EGuideThreadType::kBrand;
}

AGuideThread_Area::AGuideThread_Area(
	const FObjectInitializer& ObjectInitializer
):
 Super(
	 ObjectInitializer.
	 SetDefaultSubobjectClass<
		 UGuideAreaThreadSystemStateTreeComponent>(
		 UGuideAreaThreadSystemStateTreeComponent::ComponentName
	 )
 )
{
}

inline EGuideThreadType AGuideThread_Area::GetGuideThreadType() const
{
	return EGuideThreadType::kArea;
}

UStateTreeGuideAreaThreadComponentSchema::UStateTreeGuideAreaThreadComponentSchema()
{
	ContextActorClass = FOwnerType::StaticClass();

	ContextDataDescs[0].Struct = ContextActorClass.Get();
}

TSubclassOf<UStateTreeSchema> UGuideAreaThreadSystemStateTreeComponent::GetSchema() const
{
	return FSchemaType::StaticClass();
}

bool UGuideAreaThreadSystemStateTreeComponent::SetContextRequirements(
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

AGuideThread::AGuideThread(
	const FObjectInitializer& ObjectInitializer
):
 Super(
	 ObjectInitializer.SetDefaultSubobjectClass<
		 UGuideThreadSystemStateTreeComponent>(
		 UGuideThreadSystemStateTreeComponent::ComponentName
	 )
 )
{
}

void AGuideThread::UpdateCurrentTaskNode(
	const TSoftObjectPtr<UPAD_TaskNode_Guide>& InTaskNode
)
{
	// TaskNodeRef = InTaskNode;
	// OnCurrentTaskNodeChanged.Broadcast(TaskNodeRef);
}

void AGuideThread::UpdateCurrentTaskNode(
	const FTaskNodeDescript& TaskNodeDescript
)
{
	CurrentTaskNodeDescript = TaskNodeDescript;
	OnCurrentTaskNodeChanged.Broadcast(
		CurrentTaskNodeDescript
	);
}

void AGuideThread::AddEvent(
	const FTaskNodeResuleHelper& TaskNodeResuleHelper
)
{
	EventsSet.Add(
		TaskNodeResuleHelper.TaskID,
		TaskNodeResuleHelper
	);
}

FTaskNodeResuleHelper AGuideThread::ConsumeEvent(
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

FGuid AGuideThread::GetPreviousTaskID() const
{
	return PreviousTaskID;
}

void AGuideThread::SetPreviousTaskID(
	const FGuid& PreviousGuideID_
)
{
	PreviousTaskID = PreviousGuideID_;
}

FGuid AGuideThread::GetCurrentTaskID() const
{
	return CurrentTaskID;
}

void AGuideThread::SetCurrentTaskID(
	const FGuid& TaskID
)
{
	CurrentTaskID = TaskID;
}

FGuid AGuideThread::GetGuideID() const
{
	return GuideID;
}
