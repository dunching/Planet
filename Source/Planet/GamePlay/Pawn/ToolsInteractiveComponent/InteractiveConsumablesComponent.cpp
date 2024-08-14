
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
	ConsumablesMap = InToolInfoMap;

	auto OnwerActorPtr = GetOwner<ACharacterBase>();
	auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
	auto GameplayAbilitySpecPtr = ASCPtr->FindAbilitySpecFromHandle(Skill_Consumable_GenericHandle);
	if (!GameplayAbilitySpecPtr)
	{
		return;
	}
	auto GAInsPtr = Cast<USkill_Consumable_Generic>(GameplayAbilitySpecPtr->GetPrimaryInstance());
	if (!GAInsPtr)
	{
		return;
	}

	for (const auto Iter : ConsumablesMap)
	{
		Iter.Value->UnitPtr->GAInstPtr = GAInsPtr;
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

		FGameplayEventData Payload;

		auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_Consumable_Generic;
		GameplayAbilityTargetDashPtr->UnitPtr = (*ToolIter)->UnitPtr;
		Payload.TargetData.Add(GameplayAbilityTargetDashPtr);

		auto ASCPtr = OnwerActorPtr->GetAbilitySystemComponent();
		return ASCPtr->TriggerAbilityFromGameplayEvent(
			Skill_Consumable_GenericHandle,
			ASCPtr->AbilityActorInfo.Get(),
			FGameplayTag::EmptyTag,
			&Payload,
			*ASCPtr
		);
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

		Skill_Consumable_GenericHandle = GASPtr->GiveAbility(
			FGameplayAbilitySpec(Skill_Consumable_GenericClass, 1)
		);
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
