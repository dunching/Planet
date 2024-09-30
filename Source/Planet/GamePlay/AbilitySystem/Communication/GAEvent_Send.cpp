
#include "GAEvent_Send.h"

#include "AbilitySystemComponent.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"

#include "BaseFeatureComponent.h"
#include "CS_PeriodicPropertyModify.h"
#include "CS_RootMotion.h"
#include "CS_PeriodicStateModify.h"

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
		case EEventType::kNormal:
		{
			// 外部需要这个修改的内容，所以我们这里修改CurrentEventData，而非const TriggerEventData
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

			CharacterPtr->GetBaseFeatureComponent()->OnSendEventModifyData(*ClonePtr);

			for (const auto& Iter : GAEventDataPtr->DataAry)
			{
				FGameplayAbilityTargetData_GAReceivedEvent* GAEventData =
					new FGameplayAbilityTargetData_GAReceivedEvent(Iter.TargetCharacterPtr, CharacterPtr);

				GAEventData->Data = Iter;

				FGameplayEventData Payload;
				Payload.TargetData.Add(GAEventData_EventTypePtr->Clone());
				Payload.TargetData.Add(GAEventData);

				if (!Iter.TargetCharacterPtr.IsValid())
				{
					return;
				}
				auto ASCPtr = Iter.TargetCharacterPtr->GetAbilitySystemComponent();
				ASCPtr->TriggerAbilityFromGameplayEvent(
					Iter.TargetCharacterPtr->GetBaseFeatureComponent()->ReceivedEventHandle,
					ASCPtr->AbilityActorInfo.Get(),
					FGameplayTag(),
					&Payload,
					*ASCPtr
				);
			}
		}
		break;
		case EEventType::kRootMotion:
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

			FGameplayEventData Payload;
			Payload.TargetData.Add(GAEventData_EventTypePtr->Clone());
			Payload.TargetData.Add(ClonePtr);

			if (!GAEventDataPtr->TargetCharacterPtr.IsValid())
			{
				return;
			}
			auto ASCPtr = GAEventDataPtr->TargetCharacterPtr->GetAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				GAEventDataPtr->TargetCharacterPtr->GetBaseFeatureComponent()->ReceivedEventHandle,
				ASCPtr->AbilityActorInfo.Get(),
				FGameplayTag(),
				&Payload,
				*ASCPtr
			);
		}
		break;
		case EEventType::kPeriodic_PropertyModify:
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

			auto ClonePtr = GAEventDataPtr->Clone();

			FGameplayEventData Payload;
			Payload.TargetData.Add(GAEventData_EventTypePtr->Clone());
			Payload.TargetData.Add(ClonePtr);

			if (!GAEventDataPtr->TargetCharacterPtr.IsValid())
			{
				return;
			}
			auto ASCPtr = GAEventDataPtr->TargetCharacterPtr->GetAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				GAEventDataPtr->TargetCharacterPtr->GetBaseFeatureComponent()->ReceivedEventHandle,
				ASCPtr->AbilityActorInfo.Get(),
				FGameplayTag(),
				&Payload,
				*ASCPtr
			);
		}
		break;
		case EEventType::kPeriodic_StateTagModify:
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

			FGameplayEventData Payload;
			Payload.TargetData.Add(GAEventData_EventTypePtr->Clone());
			Payload.TargetData.Add(ClonePtr);

			if (!GAEventDataPtr->TargetCharacterPtr.IsValid())
			{
				return;
			}
			auto ASCPtr = GAEventDataPtr->TargetCharacterPtr->GetAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				GAEventDataPtr->TargetCharacterPtr->GetBaseFeatureComponent()->ReceivedEventHandle,
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

