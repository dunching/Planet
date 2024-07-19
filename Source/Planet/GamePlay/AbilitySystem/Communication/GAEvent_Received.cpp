
#include "GAEvent_Received.h"

#include "AbilitySystemComponent.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "InteractiveToolComponent.h"
#include "CharacterAttributesComponent.h"
#include "InteractiveBaseGAComponent.h"
#include "GA_Periodic_StateTagModify.h"
#include "GA_Periodic_PropertyModify.h"

UGAEvent_Received::UGAEvent_Received() :
	Super()
{

}

void UGAEvent_Received::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ON_SCOPE_EXIT
	{
	K2_EndAbility();
	};

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(1))
	{
		auto GAEventData_EventTypePtr = dynamic_cast<FGameplayAbilityTargetData_GAEventType*>(CurrentEventData.TargetData.Get(0));
		if (!GAEventData_EventTypePtr)
		{
			return;
		}
		switch (GAEventData_EventTypePtr->EventType)
		{
		case FGameplayAbilityTargetData_GAEventType::EEventType::kNormal:
		{
			auto GAEventDataPtr = dynamic_cast<const FGameplayAbilityTargetData_GAReceivedEvent*>(TriggerEventData->TargetData.Get(1));
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
		break;
		case FGameplayAbilityTargetData_GAEventType::EEventType::kPeriodic_PropertyModify:
		{
			auto GAEventDataPtr = dynamic_cast<FGameplayAbilityTargetData_Periodic_PropertyModify*>(CurrentEventData.TargetData.Get(1));
			if (!GAEventDataPtr)
			{
				return;
			}

			auto CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
			if (!CharacterPtr)
			{
				return;
			}

			CharacterPtr->GetInteractiveBaseGAComponent()->ExcuteEffects(GAEventDataPtr);
		}
		break;
		case FGameplayAbilityTargetData_GAEventType::EEventType::kPeriodic_StateTagModify:
		{
			auto GAEventDataPtr = dynamic_cast<FGameplayAbilityTargetData_Periodic_StateTagModify*>(CurrentEventData.TargetData.Get(1));
			if (!GAEventDataPtr)
			{
				return;
			}

			auto CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
			if (!CharacterPtr)
			{
				return;
			}

			CharacterPtr->GetInteractiveBaseGAComponent()->ExcuteEffects(GAEventDataPtr);
		}
		break;
		}
	}
}

