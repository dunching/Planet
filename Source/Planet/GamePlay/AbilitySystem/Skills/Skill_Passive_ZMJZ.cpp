
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
		AbilityActivatedCallbacksHandle = CharacterPtr->GetAbilitySystemComponent()->AbilityActivatedCallbacks.AddLambda([this, Spec](UGameplayAbility* GAPtr) {
			auto CharacterPtr = Cast<ACharacterBase>(GAPtr->GetActorInfo().AvatarActor.Get());
			if (CharacterPtr)
			{
				if (
					GAPtr &&
					(GAPtr->GetCurrentAbilitySpecHandle() == CharacterPtr->GetEquipmentItemsComponent()->SendEventHandle)
					)
				{
					auto ASCPtr = CharacterPtr->GetAbilitySystemComponent();
					ASCPtr->TryActivateAbility(Spec.Handle);
				}
			}
			});
	}
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

	RepeatType = ERepeatType::kCount;
	RepeatCount = 1;
	CurrentRepeatCount = 0;
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

void USkill_Passive_ZMJZ::ExcuteStepsLink()
{
	if (CharacterPtr)
	{
		auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
		TaskPtr->SetIntervalTime(CountDown);
		TaskPtr->TickDelegate.BindLambda([this](UAbilityTask_TimerHelper* TaskPtr, float) {

			ModifyCount--;

			auto CharacterPtr = Cast<ACharacterBase>(GetActorInfo().AvatarActor.Get());
			if (CharacterPtr)
			{
				CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().GAPerformSpeed.AddCurrentValue(-SpeedOffset);
			}

			if (ModifyCount == 0)
			{
				DecrementListLockOverride();

				if (EffectItemPtr)
				{
					EffectItemPtr->RemoveFromParent();
					EffectItemPtr = nullptr;

					ModifyCount = 0;
				}
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

		IncrementListLock();

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

