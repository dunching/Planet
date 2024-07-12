
#include "Skill_Consumable_Base.h"

#include "AbilitySystemComponent.h"

void USkill_Consumable_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

bool USkill_Consumable_Base::CanUse() const
{
	return false;
}
