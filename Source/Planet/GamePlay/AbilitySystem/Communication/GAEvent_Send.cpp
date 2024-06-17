
#include "GAEvent_Send.h"

#include "AbilitySystemComponent.h"

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "EquipmentElementComponent.h"

UGAEvent_Send::UGAEvent_Send() :
	Super()
{

}

void UGAEvent_Send::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ON_SCOPE_EXIT
	{
	K2_EndAbility();
	};

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		auto GAEventDataPtr = dynamic_cast<const FGameplayAbilityTargetData_GASendEvent*>(TriggerEventData->TargetData.Get(0));
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

		CharacterPtr->GetEquipmentItemsComponent()->OnSendEventModifyData(*Clone);

		for (const auto & Iter : Clone->DataAry)
		{
			FGameplayAbilityTargetData_GAReceivedEvent* GAEventData =
				new FGameplayAbilityTargetData_GAReceivedEvent(Iter.TargetCharacterPtr, CharacterPtr);

			GAEventData->Data = Iter;

			FGameplayEventData Payload;
			Payload.TargetData.Add(GAEventData);

			auto ASCPtr = Iter.TargetCharacterPtr->GetAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				Iter.TargetCharacterPtr->GetEquipmentItemsComponent()->ReceivedEventHandle,
				ASCPtr->AbilityActorInfo.Get(),
				FGameplayTag(),
				&Payload,
				*ASCPtr
			);
		}
	}
}

