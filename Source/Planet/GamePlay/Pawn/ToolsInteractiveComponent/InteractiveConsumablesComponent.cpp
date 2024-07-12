
#include "InteractiveConsumablesComponent.h"

#include "Consumable_Test.h"
#include "CharacterBase.h"
#include "Skill_Consumable_Base.h"
#include "Skill_Consumable_Test.h"
#include "Skill_Consumable_Generic.h"

FName UInteractiveConsumablesComponent::ComponentName = TEXT("InteractiveConsumablesComponent");

void UInteractiveConsumablesComponent::RegisterConsumable(
	const TMap <FGameplayTag, TSharedPtr<FConsumableSocketInfo>>& InToolInfoMap, bool bIsGenerationEvent /*= true */
)
{
	ToolsMap = InToolInfoMap;

	if (bIsGenerationEvent)
	{
		GenerationCanbeActiveEvent();
	}
}

TArray<TSharedPtr<FCanbeActivedInfo>> UInteractiveConsumablesComponent::GetCanbeActiveAction() const
{
	return CanbeActiveToolsAry;
}

bool UInteractiveConsumablesComponent::ActiveAction(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr, bool bIsAutomaticStop /*= false */)
{
	auto ToolIter = ToolsMap.Find(CanbeActivedInfoSPtr->Socket);
	if (!ToolIter)
	{
		return  false;
	}

	switch ((*ToolIter)->UnitPtr->GetSceneElementType<EConsumableUnitType>())
	{
	case EConsumableUnitType::kTest:
	{
		auto OnwerActorPtr = GetOwner<ACharacterBase>();
		if (!OnwerActorPtr)
		{
			return  false;
		}

		FGameplayEventData Payload;

		auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Consumable_Test;
		GameplayAbilityTargetDashPtr->UnitPtr = (*ToolIter)->UnitPtr;
		Payload.TargetData.Add(GameplayAbilityTargetDashPtr);

		FGameplayAbilitySpec GameplayAbilitySpec(
			(*ToolIter)->UnitPtr->Skill_Consumable_Class, 
			1
		);
		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
		ASCPtr->TriggerAbilityFromGameplayEvent(
			Skill_Consumable_GenericHandle,
			ASCPtr->AbilityActorInfo.Get(),
			FGameplayTag::EmptyTag,
			&Payload,
			*ASCPtr
		);

		return true;
	}
	break;
	}

	return false;
}

void UInteractiveConsumablesComponent::CancelAction(const TSharedPtr<FCanbeActivedInfo>& CanbeActivedInfoSPtr)
{

}

TSharedPtr<FConsumableSocketInfo> UInteractiveConsumablesComponent::FindConsumable(const FGameplayTag& Tag)
{
	auto Iter = ToolsMap.Find(Tag);
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

		Skill_Consumable_GenericHandle = GASPtr->GiveAbility(
			FGameplayAbilitySpec(Skill_Consumable_GenericClass, 1)
		);
	}
}

void UInteractiveConsumablesComponent::GenerationCanbeActiveEvent()
{
	CanbeActiveToolsAry.Empty();

	for (const auto& Iter : ToolsMap)
	{
		if (Iter.Value->UnitPtr)
		{
			TSharedPtr<FCanbeActivedInfo > CanbeActivedInfoSPtr = MakeShared<FCanbeActivedInfo>();
			CanbeActivedInfoSPtr->Type = FCanbeActivedInfo::EType::kConsumables;
			CanbeActivedInfoSPtr->Key = Iter.Value->Key;
			CanbeActivedInfoSPtr->Socket = Iter.Value->SkillSocket;

			CanbeActiveToolsAry.Add(CanbeActivedInfoSPtr);
		}
	}
}
