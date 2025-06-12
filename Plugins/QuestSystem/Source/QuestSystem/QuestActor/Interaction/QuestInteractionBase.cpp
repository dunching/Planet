#include "QuestInteractionBase.h"

#include "GameFramework/Character.h"

#include "STT_QuestInteractionBase.h"

const FName PlayerCharacter(TEXT("PlayerCharacter"));

AQuestInteractionBase::AQuestInteractionBase(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(
	       ObjectInitializer.SetDefaultSubobjectClass<UQuestInteractionSystemSTComponentBase>(
		        UQuestInteractionSystemSTComponentBase::ComponentName
		       )
	      )
{
}

const FName HumanCharacter_NPC_Target(TEXT("HumanCharacter_NPC_Target"));

USTQuestInteractionComponentSchemaBase::USTQuestInteractionComponentSchemaBase()
{
	check(ContextDataDescs.Num() == 1 && ContextDataDescs[0].Struct == AActor::StaticClass());

	ContextActorClass = FOwnerType::StaticClass();

	ContextDataDescs[0].Struct = ContextActorClass.Get();
	ContextDataDescs.Emplace(
	                         PlayerCharacter,
	                         ACharacterClass.Get(),
	                         FGuid::NewGuid()
	                        );
	ContextDataDescs.Emplace(
	                         PlayerCharacter,
	                         InteractionTargetClass.Get(),
	                         FGuid::NewGuid()
	                        );
}

void USTQuestInteractionComponentSchemaBase::PostLoad()
{
	Super::PostLoad();

	ContextDataDescs[1].Struct = ACharacterClass.Get();
	ContextDataDescs[2].Struct = InteractionTargetClass.Get();
}

bool USTQuestInteractionComponentSchemaBase::IsStructAllowed(
	const UScriptStruct* InScriptStruct
	) const
{
	return Super::IsStructAllowed(InScriptStruct) ||
	       InScriptStruct->IsChildOf(FSTT_QuestInteractionBase::StaticStruct()) ||
	       InScriptStruct->IsChildOf(
	                                 FSTT_Guide_Record::StaticStruct()
	                                );
}

bool USTQuestInteractionComponentSchemaBase::SetContextRequirements(
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
		// TODO
	}

	return Super::SetContextRequirements(BrainComponent, Context, bLogErrors);
}

TSubclassOf<UStateTreeSchema> UQuestInteractionSystemSTComponentBase::GetSchema() const
{
	return FSchemaType::StaticClass();
}

bool UQuestInteractionSystemSTComponentBase::SetContextRequirements(
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
