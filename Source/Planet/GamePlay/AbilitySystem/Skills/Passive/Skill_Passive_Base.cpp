
#include "Skill_Passive_Base.h"

#include "SceneUnitTable.h"
#include "CharacterBase.h"
#include "InteractiveBaseGAComponent.h"
#include "CharacterAttibutes.h"
#include "GenerateType.h"
#include "CharacterAttributesComponent.h"
#include "PropertyEntrys.h"

USkill_Passive_Base::USkill_Passive_Base() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void USkill_Passive_Base::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CallActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());

	if (CharacterPtr)
	{
		auto PassiveSkillUnitPtr = Cast<UPassiveSkillUnit>(SkillUnitPtr);

		// 元素
		auto PassiveSkillExtendInfoPtr = PassiveSkillUnitPtr->GetTableRowUnit_PassiveSkillExtendInfo();
		if (PassiveSkillExtendInfoPtr)
		{
			TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

			for (const auto& ElementIter : PassiveSkillExtendInfoPtr->AddtionalElementMap)
			{
				ModifyPropertyMap.Add(ElementIter.Key, ElementIter.Value);
			}

			CharacterPtr->GetInteractiveBaseGAComponent()->SendEvent2Self(
				ModifyPropertyMap, PassiveSkillUnitPtr->GetUnitType()
			);
		}

		// 词条
		{
			auto MainPropertyEntryPtr = PassiveSkillUnitPtr->GetMainPropertyEntry();
			if (MainPropertyEntryPtr)
			{
				TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

				ModifyPropertyMap.Add(MainPropertyEntryPtr->CharacterPropertyType, MainPropertyEntryPtr->Value);

				CharacterPtr->GetInteractiveBaseGAComponent()->SendEvent2Self(
					ModifyPropertyMap, PassiveSkillUnitPtr->GetUnitType()
				);
			}
		}
	}
}

void USkill_Passive_Base::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	if (CharacterPtr)
	{
		auto PassiveSkillUnitPtr = Cast<UPassiveSkillUnit>(SkillUnitPtr);
		CharacterPtr->GetInteractiveBaseGAComponent()->ClearData2Self(
			GetAllData(), PassiveSkillUnitPtr->GetUnitType()
		);
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}
