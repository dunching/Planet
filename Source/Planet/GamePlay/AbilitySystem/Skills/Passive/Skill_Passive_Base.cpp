
#include "Skill_Passive_Base.h"

#include "SceneUnitTable.h"
#include "CharacterBase.h"
#include "BaseFeatureComponent.h"
#include "CharacterAttibutes.h"
#include "GenerateType.h"
#include "CharacterAttributesComponent.h"
#include "PropertyEntrys.h"
#include "TemplateHelper.h"

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
		auto PassiveSkillUnitPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(SkillUnitPtr);
	}
}

void USkill_Passive_Base::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	if (CharacterPtr)
	{
		auto PassiveSkillUnitPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(SkillUnitPtr);
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}
