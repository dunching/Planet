
#include "Skill_Talent_Base.h"

#include "AbilitySystemComponent.h"

FTalent_Base::~FTalent_Base()
{

}

void USkill_Talent_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
}
