#include "QuestChain.h"

#include "Kismet/GameplayStatics.h"
#include "GameplayTasksComponent.h"
#include "StateTreeConditionBase.h"

#include "STT_QuestChain.h"

#include "QuestSystemStateTreeComponent.h"
#include "LogWriter.h"
#include "PlanetPlayerController.h"

const FName PlayerController(
                             TEXT(
                                  "PlayerController"
                                 )
                            );

AQuestChain_Main::AQuestChain_Main(
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

AQuestChain_Area::AQuestChain_Area(
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

AQuestChain::AQuestChain(
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
