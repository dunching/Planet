
#include "Skill_Element_Base.h"

#include "AbilitySystemComponent.h"

void USkill_Element_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
}
