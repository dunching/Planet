
#include "GAEvent_Received.h"

#include "AbilitySystemComponent.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "InteractiveToolComponent.h"
#include "CharacterAttributesComponent.h"
#include "InteractiveBaseGAComponent.h"
#include "CS_RootMotion.h"
#include "CS_PeriodicPropertyModify.h"
#include "CS_PeriodicStateModify.h"

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
		auto GAEventData_EventTypePtr = dynamic_cast<const FGameplayAbilityTargetData_GAEventType*>(TriggerEventData->TargetData.Get(0));
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

			auto CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
			if (!CharacterPtr)
			{
				return;
			}

			auto CloneSPtr = GAEventDataPtr->Clone_SmartPtr();

			CharacterPtr->GetInteractiveBaseGAComponent()->OnReceivedEventModifyData(*CloneSPtr);

			CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().ProcessGAEVent(*CloneSPtr);

			CloneSPtr->TrueDataDelagate.ExcuteCallback(CharacterPtr, CloneSPtr->Data);

			GAEventDataPtr->TriggerCharacterPtr->GetInteractiveBaseGAComponent()->MakedDamage.ExcuteCallback(CharacterPtr, CloneSPtr->Data);

			if (GAEventDataPtr->Data.bIsMakeAttackEffect)
			{
				CharacterPtr->GetInteractiveBaseGAComponent()->ExcuteAttackedEffect(EAffectedDirection::kForward);
			}
		}
		break;
		case FGameplayAbilityTargetData_GAEventType::EEventType::kRootMotion:
		{
			auto GAEventDataPtr = dynamic_cast<const FGameplayAbilityTargetData_RootMotion*>(TriggerEventData->TargetData.Get(1));
			if (!GAEventDataPtr)
			{
				return;
			}

			auto CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
			if (!CharacterPtr)
			{
				return;
			}

			auto ClonePtr = GAEventDataPtr->Clone();

			CharacterPtr->GetInteractiveBaseGAComponent()->ExcuteEffects(ClonePtr);
		}
		break;
		case FGameplayAbilityTargetData_GAEventType::EEventType::kPeriodic_PropertyModify:
		{
			auto GAEventDataPtr = dynamic_cast<const FGameplayAbilityTargetData_PropertyModify*>(TriggerEventData->TargetData.Get(1));
			if (!GAEventDataPtr)
			{
				return;
			}

			auto CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
			if (!CharacterPtr)
			{
				return;
			}

			auto ClonePtr = TSharedPtr<FGameplayAbilityTargetData_PropertyModify>(GAEventDataPtr->Clone());

			CharacterPtr->GetInteractiveBaseGAComponent()->ExcuteEffects(ClonePtr);
		}
		break;
		case FGameplayAbilityTargetData_GAEventType::EEventType::kPeriodic_StateTagModify:
		{
			auto GAEventDataPtr = dynamic_cast<const FGameplayAbilityTargetData_StateModify*>(TriggerEventData->TargetData.Get(1));
			if (!GAEventDataPtr)
			{
				return;
			}

			auto CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
			if (!CharacterPtr)
			{
				return;
			}

			auto ClonePtr = GAEventDataPtr->Clone();

			CharacterPtr->GetInteractiveBaseGAComponent()->ExcuteEffects(ClonePtr);
		}
		break;
		}
	}
}

