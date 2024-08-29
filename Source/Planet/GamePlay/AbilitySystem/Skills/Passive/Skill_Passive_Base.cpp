
#include "Skill_Passive_Base.h"

#include "SceneUnitTable.h"
#include "CharacterBase.h"
#include "InteractiveBaseGAComponent.h"
#include "CharacterAttibutes.h"
#include "GenerateType.h"
#include "CharacterAttributesComponent.h"

void USkill_Passive_Base::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CallActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());

	if (CharacterPtr)
	{
		auto PassiveSkillUnitPtr = Cast<UPassiveSkillUnit>(SkillUnitPtr);
		auto PassiveSkillExtendInfoPtr = PassiveSkillUnitPtr->GetTableRowUnit_PassiveSkillExtendInfo();
		if (PassiveSkillExtendInfoPtr)
		{
			TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

			for (const auto& ElementIter : PassiveSkillExtendInfoPtr->AddtionalElementMap)
			{
				switch (ElementIter.Key)
				{
				case EWuXingType::kGold:
				{
					ModifyPropertyMap.Add(
						ECharacterPropertyType::GoldElement,
						CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes()->Element.GoldElement.GetCurrentValue()
					);
				}
				break;
				}
			}

			CharacterPtr->GetInteractiveBaseGAComponent()->SendEvent2Self(
				ModifyPropertyMap, PassiveSkillUnitPtr->GetUnitType()
			);
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
