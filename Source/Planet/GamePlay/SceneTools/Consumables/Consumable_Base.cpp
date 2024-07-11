
#include "Consumable_Base.h"

#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"

#include "CharacterBase.h"
#include "Skill_Consumable_Base.h"

void AConsumable_Base::Interaction(ACharacterBase* CharacterPtr)
{
	FGameplayEventData Payload;
	FGameplayAbilitySpec GameplayAbilitySpec(
		Skill_Consumables_Class,
		1
	);
	auto ASCPtr = CharacterPtr->GetAbilitySystemComponent();
	AbilitieHandle = ASCPtr->GiveAbilityAndActivateOnce(
		GameplayAbilitySpec,
		&Payload
	);
}

