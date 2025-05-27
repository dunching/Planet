
#include "Skill_Element_Base.h"

#include "AbilitySystemComponent.h"
#include "CharacterBase.h"

void USkill_Element_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	
	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

}
