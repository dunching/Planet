
#include "Skill_Passive_Base.h"

void USkill_Passive_Base::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CallActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo());
}

void USkill_Passive_Base::PerformAction()
{

}
