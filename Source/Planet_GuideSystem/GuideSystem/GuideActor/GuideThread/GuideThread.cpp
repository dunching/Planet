#include "GuideThread.h"

#include "Kismet/GameplayStatics.h"
#include "GameplayTasksComponent.h"
#include "StateTreeConditionBase.h"

#include "STT_GuideThread.h"

#include "GuideSystemStateTreeComponent.h"
#include "LogWriter.h"
#include "PlanetPlayerController.h"

const FName PlayerController(
                             TEXT(
                                  "PlayerController"
                                 )
                            );

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

AGuideThread::AGuideThread(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(
	       ObjectInitializer
	      )
{
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
