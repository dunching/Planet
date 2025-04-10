#include "GuideInteraction.h"

#include "Kismet/GameplayStatics.h"

#include "GameplayTasksComponent.h"
#include "GuideSystemStateTreeComponent.h"
#include "HumanCharacter_AI.h"
#include "HumanCharacter_Player.h"
#include "LogWriter.h"
#include "STT_GuideInteraction.h"

bool UStateTreeGuideInteractionComponentSchema::IsStructAllowed(const UScriptStruct* InScriptStruct) const
{
	return Super::IsStructAllowed(InScriptStruct) ||
		InScriptStruct->IsChildOf(FSTT_GuideInteractionBase::StaticStruct());
}

TSubclassOf<UStateTreeSchema> UGuideInteractionSystemStateTreeComponent::GetSchema() const
{
	return UStateTreeGuideInteractionComponentSchema::StaticClass();
}

AGuideInteraction_Actor::AGuideInteraction_Actor(const FObjectInitializer& ObjectInitializer):
	Super(
		ObjectInitializer.SetDefaultSubobjectClass<UGuideInteractionSystemStateTreeComponent>(
			UGuideInteractionSystemStateTreeComponent::ComponentName
		)
	)
{
	GuideStateTreeComponentPtr->SetStartLogicAutomatically(true);
}

const FName PlayerTarget(TEXT("PlayerTarget"));

const FName HumanCharacter_NPC_Target(TEXT("HumanCharacter_NPC_Target"));

UStateTreeGuideInteraction_HumanCharacter_AI_ComponentSchema::UStateTreeGuideInteraction_HumanCharacter_AI_ComponentSchema()
{
	check(ContextDataDescs.Num() == 1 && ContextDataDescs[0].Struct == AActor::StaticClass());

	ContextActorClass = FOwnerType::StaticClass();
	HumanCharacter_PlayerClass = AHumanCharacter_Player::StaticClass();
	HumanCharacter_AIClass = AHumanCharacter_AI::StaticClass();

	ContextDataDescs[0].Struct = ContextActorClass.Get();
	ContextDataDescs.Emplace(
		PlayerTarget,
		HumanCharacter_PlayerClass.Get(),
		FGuid(0xEDB3CD97, 0x95F94E0A, 0xBD15207B, 0x98645CDC)
	);
	ContextDataDescs.Emplace(
		HumanCharacter_NPC_Target,
		HumanCharacter_AIClass.Get(),
		FGuid(0xEDB3CD98, 0x95F94E0A, 0xBD15207B, 0x98645CDC)
	);
}

void UStateTreeGuideInteraction_HumanCharacter_AI_ComponentSchema::PostLoad()
{
	Super::PostLoad();

	ContextDataDescs[1].Struct = HumanCharacter_PlayerClass.Get();
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
		auto Character_Player = Cast<AHumanCharacter_Player>(UGameplayStatics::GetPlayerCharacter(OwnerPtr, 0));

		Context.SetContextDataByName(PlayerTarget, FStateTreeDataView(Character_Player));
		Context.SetContextDataByName(HumanCharacter_NPC_Target, FStateTreeDataView(OwnerPtr->Character_NPC));
	}

	return Super::SetContextRequirements(BrainComponent, Context, bLogErrors);
}

TSubclassOf<UStateTreeSchema> UGuideInteraction_HumanCharacter_AI_SystemStateTreeComponent::GetSchema() const
{
	return UStateTreeGuideInteraction_HumanCharacter_AI_ComponentSchema::StaticClass();
}

bool UGuideInteraction_HumanCharacter_AI_SystemStateTreeComponent::SetContextRequirements(
	FStateTreeExecutionContext& Context, bool bLogErrors)
{
	Context.SetCollectExternalDataCallback(
		FOnCollectStateTreeExternalData::CreateUObject(this, &ThisClass::CollectExternalData)
		);
	return UStateTreeGuideInteraction_HumanCharacter_AI_ComponentSchema::SetContextRequirements(
		*this,
		Context,
		bLogErrors
		);
}

AGuideInteraction_HumanCharacter_AI::AGuideInteraction_HumanCharacter_AI(const FObjectInitializer& ObjectInitializer):
	Super(
		ObjectInitializer.SetDefaultSubobjectClass<UGuideInteraction_HumanCharacter_AI_SystemStateTreeComponent>(
			UGuideInteraction_HumanCharacter_AI_SystemStateTreeComponent::ComponentName
		)
	)
{
}
