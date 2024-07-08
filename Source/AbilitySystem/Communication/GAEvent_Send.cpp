
#include "GAEvent_Send.h"

#include "AbilitySystemComponent.h"
#include <GameFramework/Character.h>

#include "GAEvent_Helper.h"
#include "InteractiveSkillComponent.h"
#include "InteractiveToolComponent.h"
#include "InteractiveBaseGAComponent.h"
#include "GAOwnerTypeInterface.h"

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
		// 外部需要这个修改的内容，所以我们这里修改CurrentEventData，而非const TriggerEventData
		auto GAEventDataPtr = dynamic_cast<FGameplayAbilityTargetData_GASendEvent*>(CurrentEventData.TargetData.Get(0));
		if (!GAEventDataPtr)
		{
			return;
		}

		if (!CharacterPtr)
		{
			return;
		}

		CharacterInterfacePtr->GetInteractiveBaseGAComponent()->OnSendEventModifyData(*GAEventDataPtr);

		for (const auto & Iter : GAEventDataPtr->DataAry)
		{
			FGameplayAbilityTargetData_GAReceivedEvent* GAEventData =
				new FGameplayAbilityTargetData_GAReceivedEvent(Iter.TargetCharacterPtr, CharacterPtr);

			GAEventData->Data = Iter;

			FGameplayEventData Payload;
			Payload.TargetData.Add(GAEventData);

			auto ASCPtr = Cast<FOwnerInterfaceType>(Iter.TargetCharacterPtr)->GetAbilitySystemComponent();
			ASCPtr->TriggerAbilityFromGameplayEvent(
				Cast<FOwnerInterfaceType>(Iter.TargetCharacterPtr)->GetInteractiveBaseGAComponent()->ReceivedEventHandle,
				ASCPtr->AbilityActorInfo.Get(),
				FGameplayTag(),
				&Payload,
				*ASCPtr
			);
		}
	}
}

