
#include "Skill_ZMJZ.h"

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

USkill_ZMJZ::USkill_ZMJZ() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;
}

void USkill_ZMJZ::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
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

void USkill_ZMJZ::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData /*= nullptr */)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	RepeatType = ERepeatType::kCount;

	CurrentRepeatCount = 0;
}

void USkill_ZMJZ::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (IsMarkPendingKillOnAbilityEnd())
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
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_ZMJZ::ExcuteStepsLink()
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

