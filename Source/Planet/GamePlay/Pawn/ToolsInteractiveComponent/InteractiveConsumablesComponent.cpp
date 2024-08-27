
#include "InteractiveConsumablesComponent.h"

#include "Consumable_Test.h"
#include "CharacterBase.h"
#include "Skill_Consumable_Base.h"
#include "Skill_Consumable_Test.h"
#include "Skill_Consumable_Generic.h"
#include "GameplayTagsSubSystem.h"
#include "SceneUnitTable.h"

FName UInteractiveConsumablesComponent::ComponentName = TEXT("InteractiveConsumablesComponent");

void UInteractiveConsumablesComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInteractiveConsumablesComponent::RegisterConsumable(
	const TMap <FGameplayTag, TSharedPtr<FConsumableSocketInfo>>& InToolInfoMap, bool bIsGenerationEvent /*= true */
)
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (!OnwerActorPtr)
	{
		return;
	}

	for (auto Iter : ConsumablesMap)
	{
		Iter.Value->UnitPtr->SetAllocationCharacterUnit(nullptr);
	}

	ConsumablesMap = InToolInfoMap;

	for (auto Iter : ConsumablesMap)
	{
		Iter.Value->UnitPtr->SetAllocationCharacterUnit(OnwerActorPtr->GetCharacterUnit());
	}

	if (bIsGenerationEvent)
	{
		GenerationCanbeActiveEvent();
	}
}

bool UInteractiveConsumablesComponent::ActiveAction(const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false */)
{
	auto ToolIter = ConsumablesMap.Find(CanbeActivedInfoSPtr->Socket);
	if (!ToolIter)
	{
		return  false;
	}

	if (
		(*ToolIter)->UnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables_HP) ||
		(*ToolIter)->UnitPtr->GetUnitType().MatchesTag(UGameplayTagsSubSystem::GetInstance()->Unit_Consumables_PP)
		)
	{
		auto OnwerActorPtr = GetOwner<ACharacterBase>();
		if (!OnwerActorPtr)
		{
			return  false;
		}

		FGameplayAbilitySpec GameplayAbilitySpec(
			(*ToolIter)->UnitPtr->GetTableRowUnit_Consumable()->Skill_Consumable_Class,
			1
		); 
		GameplayAbilitySpec.GameplayEventData = MakeShared<FGameplayEventData>();
		auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Consumable;
		GameplayAbilityTargetDashPtr->UnitPtr = (*ToolIter)->UnitPtr;
		GameplayAbilitySpec.GameplayEventData->TargetData.Add(GameplayAbilityTargetDashPtr);

		auto GASPtr = OnwerActorPtr->GetAbilitySystemComponent();

		GASPtr->GiveAbilityAndActivateOnce(
			GameplayAbilitySpec,
			nullptr
		);

		return true;
	}

	return false;
}

void UInteractiveConsumablesComponent::CancelAction(const TSharedPtr<FCanbeInteractionInfo>& CanbeActivedInfoSPtr)
{

}

TSharedPtr<FConsumableSocketInfo> UInteractiveConsumablesComponent::FindConsumable(const FGameplayTag& SocketTag)
{
	auto Iter = ConsumablesMap.Find(SocketTag);
	if (Iter)
	{
		return *Iter;
	}

	return nullptr;
}

void UInteractiveConsumablesComponent::InitialBaseGAs()
{
	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	if (OnwerActorPtr)
	{
		auto GASPtr = OnwerActorPtr->GetAbilitySystemComponent();
	}
}

void UInteractiveConsumablesComponent::GenerationCanbeActiveEvent()
{
	CanbeInteractionAry.Empty();

	for (const auto& Iter : ConsumablesMap)
	{
		TSharedPtr<FCanbeInteractionInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeInteractionInfo>();
		CanbeActivedInfoSPtr->Type = FCanbeInteractionInfo::EType::kConsumables;
		CanbeActivedInfoSPtr->Key = Iter.Value->Key;
		CanbeActivedInfoSPtr->Socket = Iter.Value->SkillSocket;

		CanbeInteractionAry.Add(CanbeActivedInfoSPtr);
	}
}
