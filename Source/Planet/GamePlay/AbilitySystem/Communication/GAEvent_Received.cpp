
#include "GAEvent_Received.h"

#include "AbilitySystemComponent.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "InteractiveToolComponent.h"
#include "CharacterAttributesComponent.h"
#include "InteractiveBaseGAComponent.h"

UGAEvent_Received::UGAEvent_Received() :
	Super()
{

}

void UGAEvent_Received::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ON_SCOPE_EXIT
	{
	K2_EndAbility();
	};

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		auto GAEventDataPtr = dynamic_cast<const FGameplayAbilityTargetData_GAReceivedEvent*>(TriggerEventData->TargetData.Get(0));
		if (!GAEventDataPtr)
		{
			return;
		}

		auto Clone = GAEventDataPtr->Clone();

		auto CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
		if (!CharacterPtr)
		{
			return;
		}

		CharacterPtr->GetInteractiveBaseGAComponent()->OnReceivedEventModifyData(*Clone);

		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().ProcessGAEVent(*Clone);

		Clone->TrueDataDelagate.ExcuteCallback(CharacterPtr, Clone->Data);
	}
}

