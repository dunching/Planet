
#include "Skill_Passive_Base.h"

#include "SceneProxyTable.h"
#include "CharacterBase.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterAttibutes.h"
#include "GenerateTypes.h"
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
		auto PassiveSkillProxyPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(SkillProxyPtr);
	}
}

void USkill_Passive_Base::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	if (CharacterPtr)
	{
		auto PassiveSkillProxyPtr = DynamicCastSharedPtr<FPassiveSkillProxy>(SkillProxyPtr);
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}
