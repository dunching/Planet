
#include "Skill_Element_Gold.h"

#include "AbilitySystemComponent.h"

#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "AbilityTask_TimerHelper.h"
#include "CharacterAbilitySystemComponent.h"

void USkill_Element_Gold::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (CharacterPtr)
	{
		// auto CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		// OnValueChanged = CharacterAttributes.GoldElement.AddOnValueChanged(
		// 	std::bind(&ThisClass::OnElementLevelChanged, this, std::placeholders::_1, std::placeholders::_2)
		// );

		AbilityActivatedCallbacksHandle =
			CharacterPtr->GetCharacterAbilitySystemComponent()->AbilityActivatedCallbacks.AddUObject(this, &ThisClass::MakedDamageDelegate);
	}
}


void USkill_Element_Gold::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	CharacterPtr->GetCharacterAbilitySystemComponent()->AbilityActivatedCallbacks.Remove(AbilityActivatedCallbacksHandle);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Element_Gold::OnElementLevelChanged(int32 OldValue, int32 NewValue)
{
	CurrentElementLevel = NewValue;
}

void USkill_Element_Gold::MakedDamageDelegate(UGameplayAbility* GAPtr)
{
}

void USkill_Element_Gold::AddBuff()
{
	switch (CurrentElementLevel)
	{
	case 3:
	{
	}
	break;
	case 6:
	{
	}
	break;
	case 9:
	{
	}
	break;
	}
}

void USkill_Element_Gold::RemoveBuff()
{
	if (CharacterPtr)
	{
	}
}
