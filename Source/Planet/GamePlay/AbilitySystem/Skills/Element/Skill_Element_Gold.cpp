
#include "Skill_Element_Gold.h"

#include "AbilitySystemComponent.h"

#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "AbilityTask_TimerHelper.h"
#include "InteractiveBaseGAComponent.h"
#include "GameplayTagsSubSystem.h"

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
		auto CharacterAttributesSPtr = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
		OnValueChanged = CharacterAttributesSPtr->GoldElement.AddOnValueChanged(
			std::bind(&ThisClass::OnElementLevelChanged, this, std::placeholders::_1, std::placeholders::_2)
		);

		AbilityActivatedCallbacksHandle =
			CharacterPtr->GetAbilitySystemComponent()->AbilityActivatedCallbacks.AddUObject(this, &ThisClass::OnSendAttack);
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
	CharacterPtr->GetAbilitySystemComponent()->AbilityActivatedCallbacks.Remove(AbilityActivatedCallbacksHandle);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Element_Gold::OnElementLevelChanged(int32 OldValue, int32 NewValue)
{
	CurrentElementLevel = NewValue;
}

void USkill_Element_Gold::OnSendAttack(UGameplayAbility* GAPtr)
{
	if (CharacterPtr)
	{
		if (!(
			GAPtr &&
			(GAPtr->GetCurrentAbilitySpecHandle() == CharacterPtr->GetInteractiveBaseGAComponent()->SendEventHandle)
			))
		{
			return;
		}

		auto GA_SendPtr = Cast<UGAEvent_Send>(GAPtr);
		if (!GA_SendPtr)
		{
			return;
		}

		const auto& EventData = GA_SendPtr->GetCurrentEventData();

		auto GAEventPtr = dynamic_cast<const FGameplayAbilityTargetData_GASendEvent*>(EventData.TargetData.Get(0));
		if (GAEventPtr && GAEventPtr->DataAry.IsValidIndex(0) && GAEventPtr->DataAry[0].bIsWeaponAttack)
		{
			auto RegisterRemoveBuff = [this]
				{
					if (RemoveBuffTask && RemoveBuffTask->IsActive())
					{
						RemoveBuffTask->ExternalCancel();
					}

					RemoveBuffTask = UAbilityTask_TimerHelper::DelayTask(this);
					RemoveBuffTask->SetDuration(CountDown);
					RemoveBuffTask->OnFinished.BindLambda([this](UAbilityTask_TimerHelper* TaskPtr) {
						RemoveBuff();
						return true;
						});
					RemoveBuffTask->TickDelegate.BindLambda([this](UAbilityTask_TimerHelper* TaskPtr, float) {

						});
					RemoveBuffTask->ReadyForActivation();
				};

			switch (CurrentElementLevel)
			{
			case 3:
			{
				RegisterRemoveBuff();
				if (CurrentBuffLevel < 3)
				{
					CurrentBuffLevel++;
					AddBuff();
				}
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
	}
}

void USkill_Element_Gold::AddBuff()
{
	switch (CurrentElementLevel)
	{
	case 3:
	{
		if (CharacterPtr)
		{
			TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

			ModifyPropertyMap.Add(ECharacterPropertyType::CriticalHitRate, CurrentBuffLevel * CriticalHitRate);
			ModifyPropertyMap.Add(ECharacterPropertyType::Evade, CurrentBuffLevel * Evade);

			CharacterPtr->GetInteractiveBaseGAComponent()->SendEvent2Self(ModifyPropertyMap, SkillUnitPtr->GetUnitType());
		}
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
		CharacterPtr->GetInteractiveBaseGAComponent()->SendEvent2Self(GetAllData(), SkillUnitPtr->GetUnitType());
	}
}
