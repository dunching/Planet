
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
		auto GAEventDataPtr = dynamic_cast<const FGameplayAbilityTargetData_GAEvent*>(TriggerEventData->TargetData.Get(0));
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

		for (auto Iter : GAEventDataPtr->TargetActorAry)
		{
			Clone->TargetActorAry = { Iter };

			FGameplayEventData Payload;
			Payload.TargetData.Add(Clone);

			auto ASCPtr = Iter->GetAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				Iter->GetEquipmentItemsComponent()->ReceivedEventHandle,
				ASCPtr->AbilityActorInfo.Get(),
				FGameplayTag(),
				&Payload,
				*ASCPtr
			);
		}
	}
}

