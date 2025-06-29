#include "QuestInteraction.h"

#include "Kismet/GameplayStatics.h"

#include "GameplayTasksComponent.h"
#include "QuestSystemStateTreeComponent.h"
#include "HumanCharacter_AI.h"
#include "HumanCharacter_Player.h"
#include "LogWriter.h"
#include "STT_QuestInteraction.h"
#include "ChallengeEntry.h"

const FName PlayerCharacter(TEXT("PlayerCharacter"));

UStateTreeGuideInteractionComponentSchema::UStateTreeGuideInteractionComponentSchema()
{
	check(ContextDataDescs.Num() == 3);

	ContextActorClass = FOwnerType::StaticClass();
	HumanCharacter_PlayerClass = AHumanCharacter_Player::StaticClass();
}

void UStateTreeGuideInteractionComponentSchema::PostLoad()
{
	Super::PostLoad();

	ContextDataDescs[1].Struct = HumanCharacter_PlayerClass.Get();
}

bool UStateTreeGuideInteractionComponentSchema::IsStructAllowed(
	const UScriptStruct* InScriptStruct
	) const
{
	return Super::IsStructAllowed(InScriptStruct) ||
	       InScriptStruct->IsChildOf(FSTT_QuestInteractionBase::StaticStruct()) ||
	       InScriptStruct->IsChildOf(
	                                 FSTT_Guide_Record::StaticStruct()
	                                );
}

bool UStateTreeGuideInteractionComponentSchema::SetContextRequirements(
	UBrainComponent& BrainComponent,
	FStateTreeExecutionContext& Context,
	bool bLogErrors
	)
{
	if (!Context.IsValid())
	{
		return false;
	}

	PRINTFUNC();

	auto OwnerPtr = BrainComponent.GetOwner<FOwnerType>();
	if (OwnerPtr)
	{
		auto Character_Player = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(OwnerPtr, 0));

		Context.SetContextDataByName(PlayerCharacter, FStateTreeDataView(Character_Player));
	}

	return Super::SetContextRequirements(BrainComponent, Context, bLogErrors);
}

TSubclassOf<UStateTreeSchema> UGuideInteractionSystemStateTreeComponent::GetSchema() const
{
	return FSchemaType::StaticClass();
}

bool UGuideInteractionSystemStateTreeComponent::SetContextRequirements(
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

AQuestInteraction::AQuestInteraction(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(
	       ObjectInitializer.SetDefaultSubobjectClass<UGuideInteractionSystemStateTreeComponent>(
		        UGuideInteractionSystemStateTreeComponent::ComponentName
		       )
	      )
{
}

const FName HumanCharacter_NPC_Target(TEXT("HumanCharacter_NPC_Target"));

UStateTreeGuideInteraction_HumanCharacter_AI_ComponentSchema::UStateTreeGuideInteraction_HumanCharacter_AI_ComponentSchema()
{
	check(ContextDataDescs.Num() == 3);

	ContextActorClass = FOwnerType::StaticClass();
	HumanCharacter_AIClass = AHumanCharacter_AI::StaticClass();
}

void UStateTreeGuideInteraction_HumanCharacter_AI_ComponentSchema::PostLoad()
{
	Super::PostLoad();

	ContextDataDescs[0].Struct = ContextActorClass.Get();
	ContextDataDescs[2].Struct = HumanCharacter_AIClass.Get();
}

bool UStateTreeGuideInteraction_HumanCharacter_AI_ComponentSchema::SetContextRequirements(
	UBrainComponent& BrainComponent,
	FStateTreeExecutionContext& Context,
	bool bLogErrors
	)
{
	if (!Context.IsValid())
	{
		return false;
	}

	PRINTFUNC();

	auto OwnerPtr = BrainComponent.GetOwner<FOwnerType>();
	if (OwnerPtr)
	{
		Context.SetContextDataByName(HumanCharacter_NPC_Target, FStateTreeDataView(OwnerPtr->Character_NPC));
	}

	return Super::SetContextRequirements(BrainComponent, Context, bLogErrors);
}

TSubclassOf<UStateTreeSchema> UGuideInteraction_HumanCharacter_AI_SystemStateTreeComponent::GetSchema() const
{
	return FSchemaType::StaticClass();
}

bool UGuideInteraction_HumanCharacter_AI_SystemStateTreeComponent::SetContextRequirements(
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

AGuideInteraction_HumanCharacter_AI::AGuideInteraction_HumanCharacter_AI(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(
	       ObjectInitializer.SetDefaultSubobjectClass<UGuideInteraction_HumanCharacter_AI_SystemStateTreeComponent>(
		        UGuideInteraction_HumanCharacter_AI_SystemStateTreeComponent::ComponentName
		       )
	      )
{
}

const FName ChallengeEntryTarget(TEXT("ChallengeEntryTarget"));

UStateTreeGuideInteraction_ChallengeEntry_ComponentSchema::UStateTreeGuideInteraction_ChallengeEntry_ComponentSchema()
{
	check(ContextDataDescs.Num() == 3);

	ContextActorClass = FOwnerType::StaticClass();
	ChallengeEntryClass = AChallengeEntry::StaticClass();
}

void UStateTreeGuideInteraction_ChallengeEntry_ComponentSchema::PostLoad()
{
	Super::PostLoad();

	ContextDataDescs[0].Struct = ContextActorClass.Get();
	ContextDataDescs[2].Struct = ChallengeEntryClass.Get();
}

bool UStateTreeGuideInteraction_ChallengeEntry_ComponentSchema::SetContextRequirements(
	UBrainComponent& BrainComponent,
	FStateTreeExecutionContext& Context,
	bool bLogErrors
	)
{
	if (!Context.IsValid())
	{
		return false;
	}

	PRINTFUNC();

	auto OwnerPtr = BrainComponent.GetOwner<FOwnerType>();
	if (OwnerPtr)
	{
		if (OwnerPtr->ChallengeEntryPtr.IsValid())
		{
			Context.SetContextDataByName(ChallengeEntryTarget, FStateTreeDataView(OwnerPtr->ChallengeEntryPtr.Get()));
		}
	}

	return Super::SetContextRequirements(BrainComponent, Context, bLogErrors);
}

TSubclassOf<UStateTreeSchema> UGuideInteraction_ChallengeEntry_SystemStateTreeComponent::GetSchema() const
{
	return FSchemaType::StaticClass();
}

bool UGuideInteraction_ChallengeEntry_SystemStateTreeComponent::SetContextRequirements(
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

AGuideInteraction_ChallengeEntry::AGuideInteraction_ChallengeEntry(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(
	       ObjectInitializer.SetDefaultSubobjectClass<UGuideInteraction_ChallengeEntry_SystemStateTreeComponent>(
		        UGuideInteraction_ChallengeEntry_SystemStateTreeComponent::ComponentName
		       )
	      )
{
}
