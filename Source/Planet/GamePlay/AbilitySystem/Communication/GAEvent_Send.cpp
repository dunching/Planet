
#include "GAEvent_Send.h"

#include "AbilitySystemComponent.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "InteractiveToolComponent.h"
#include "InteractiveBaseGAComponent.h"
#include "GA_Periodic_PropertyModify.h"
#include "GA_Periodic_StateTagModify.h"

UGAEvent_Send::UGAEvent_Send() :
	Super()
{

}

void UGAEvent_Send::ActivateAbility(
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
			// �ⲿ��Ҫ����޸ĵ����ݣ��������������޸�CurrentEventData������const TriggerEventData
			auto GAEventDataPtr = dynamic_cast<const FGameplayAbilityTargetData_GASendEvent*>(TriggerEventData->TargetData.Get(1));
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

			CharacterPtr->GetInteractiveBaseGAComponent()->OnSendEventModifyData(*ClonePtr);

			for (const auto& Iter : GAEventDataPtr->DataAry)
			{
				FGameplayAbilityTargetData_GAReceivedEvent* GAEventData =
					new FGameplayAbilityTargetData_GAReceivedEvent(Iter.TargetCharacterPtr, CharacterPtr);

				GAEventData->Data = Iter;

				FGameplayEventData Payload;
				Payload.TargetData.Add(GAEventData_EventTypePtr->Clone());
				Payload.TargetData.Add(GAEventData);

				auto ASCPtr = Iter.TargetCharacterPtr->GetAbilitySystemComponent();
				ASCPtr->TriggerAbilityFromGameplayEvent(
					Iter.TargetCharacterPtr->GetInteractiveBaseGAComponent()->ReceivedEventHandle,
					ASCPtr->AbilityActorInfo.Get(),
					FGameplayTag(),
					&Payload,
					*ASCPtr
				);
			}
		}
		break;
		case FGameplayAbilityTargetData_GAEventType::EEventType::kPeriodic_PropertyModify:
		{
			auto GAEventDataPtr = dynamic_cast<const FGameplayAbilityTargetData_Periodic_PropertyModify*>(TriggerEventData->TargetData.Get(1));
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

			FGameplayEventData Payload;
			Payload.TargetData.Add(GAEventData_EventTypePtr->Clone());
			Payload.TargetData.Add(ClonePtr);

			auto ASCPtr = GAEventDataPtr->TargetCharacterPtr->GetAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				GAEventDataPtr->TargetCharacterPtr->GetInteractiveBaseGAComponent()->ReceivedEventHandle,
				ASCPtr->AbilityActorInfo.Get(),
				FGameplayTag(),
				&Payload,
				*ASCPtr
			);
		}
		break;
		case FGameplayAbilityTargetData_GAEventType::EEventType::kPeriodic_StateTagModify:
		{
			auto GAEventDataPtr = dynamic_cast<const FGameplayAbilityTargetData_Periodic_StateTagModify*>(TriggerEventData->TargetData.Get(1));
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

			FGameplayEventData Payload;
			Payload.TargetData.Add(GAEventData_EventTypePtr->Clone());
			Payload.TargetData.Add(ClonePtr);

			auto ASCPtr = GAEventDataPtr->TargetCharacterPtr->GetAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				GAEventDataPtr->TargetCharacterPtr->GetInteractiveBaseGAComponent()->ReceivedEventHandle,
				ASCPtr->AbilityActorInfo.Get(),
				FGameplayTag(),
				&Payload,
				*ASCPtr
			);
		}
		break;
		}
	}
}

