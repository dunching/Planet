
#include "Skill_Passive_ZMJZ.h"

#include "AbilitySystemComponent.h"

#include "CharacterBase.h"
#include "EquipmentElementComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "AbilityTask_TimerHelper.h"

USkill_Passive_ZMJZ::USkill_Passive_ZMJZ() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;
}

void USkill_Passive_ZMJZ::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		AbilityActivatedCallbacksHandle =
			CharacterPtr->GetAbilitySystemComponent()->AbilityActivatedCallbacks.AddUObject(this, &ThisClass::TriggerSelf);
	}
}

void USkill_Passive_ZMJZ::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void USkill_Passive_ZMJZ::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	if (CharacterPtr)
	{
		CharacterPtr->GetAbilitySystemComponent()->AbilityActivatedCallbacks.Remove(AbilityActivatedCallbacksHandle);
		CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().GAPerformSpeed.AddCurrentValue(-(ModifyCount * SpeedOffset));
	}

	if (EffectItemPtr)
	{
		EffectItemPtr->RemoveFromParent();
		EffectItemPtr = nullptr;

		ModifyCount = 0;
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}

void USkill_Passive_ZMJZ::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData /*= nullptr */)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void USkill_Passive_ZMJZ::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Passive_ZMJZ::PerformAction()
{
	if (CharacterPtr)
	{
		auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
		TaskPtr->SetInfinite();
		TaskPtr->SetIntervalTime(CountDown);
		TaskPtr->TickDelegate.BindLambda([this](UAbilityTask_TimerHelper* TaskPtr, float) {

			ModifyCount--;

			auto CharacterPtr = Cast<ACharacterBase>(GetActorInfo().AvatarActor.Get());
			if (CharacterPtr)
			{
				CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().GAPerformSpeed.AddCurrentValue(-SpeedOffset);
			}

			if (ModifyCount <= 0)
			{
				if (EffectItemPtr)
				{
					EffectItemPtr->RemoveFromParent();
					EffectItemPtr = nullptr;
				}
				TaskPtr->ExternalCancel();
			}
			else if (TaskPtr)
			{
				if (EffectItemPtr)
				{
					EffectItemPtr->SetNum(ModifyCount);
				}
				TaskPtr->SetIntervalTime(SecondaryCountDown);
			}
			});
		TaskPtr->ReadyForActivation();

		if (ModifyCount > MaxCount)
		{
		}
		else
		{
			if (ModifyCount == 0)
			{
				auto EffectPtr = UUIManagerSubSystem::GetInstance()->ViewEffectsList(true);
				if (EffectPtr)
				{
					EffectItemPtr = EffectPtr->AddEffectItem();
					EffectItemPtr->SetTexutre(BuffIcon);
				}
			}

			ModifyCount++;
			CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().GAPerformSpeed.AddCurrentValue(SpeedOffset);

			if (EffectItemPtr)
			{
				EffectItemPtr->SetNum(ModifyCount);
			}
		}
	}
}

void USkill_Passive_ZMJZ::TriggerSelf(UGameplayAbility* GAPtr)
{
	if (CharacterPtr)
	{
		if (!(
				GAPtr &&
				(GAPtr->GetCurrentAbilitySpecHandle() == CharacterPtr->GetEquipmentItemsComponent()->SendEventHandle)
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

		auto GAEventPtr = dynamic_cast<const FGameplayAbilityTargetData_GAEvent*>(EventData.TargetData.Get(0));
		if (GAEventPtr && GAEventPtr->Data.bIsWeaponAttack)
		{
			auto ASCPtr = CharacterPtr->GetAbilitySystemComponent();
			ASCPtr->TryActivateAbility(GetCurrentAbilitySpecHandle());
		}
	}
}
