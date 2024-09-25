
#include "Skill_Passive_XS.h"

#include "AbilitySystemComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "CharacterBase.h"
#include "UnitProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "AbilityTask_TimerHelper.h"
#include "BaseFeatureComponent.h"
#include "StateProcessorComponent.h"
#include "CS_PeriodicPropertyModify.h"
#include "CS_Base.h"
#include "CharacterStateInfo.h"

void USkill_Passive_XS::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void USkill_Passive_XS::PreActivate(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData /*= nullptr */)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (CharacterPtr)
	{
		EventModifyReceivedSPtr = MakeShared<FMyStruct>(300, this);

		CharacterPtr->GetBaseFeatureComponent()->AddReceviedEventModify(EventModifyReceivedSPtr);
	}
}

void USkill_Passive_XS::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USkill_Passive_XS::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	CharacterPtr->GetBaseFeatureComponent()->RemoveReceviedEventModify(EventModifyReceivedSPtr);

	Super::OnRemoveAbility(ActorInfo, Spec);
}

void USkill_Passive_XS::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Passive_XS::PerformAction()
{
}

void USkill_Passive_XS::OnSendAttack(const FGAEventData& GAEventData)
{
}

void USkill_Passive_XS::OnIntervalTick(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval)
{
}

USkill_Passive_XS::FMyStruct::FMyStruct(int32 InPriority, USkill_Passive_XS* InGAInsPtr) :
	IGAEventModifyReceivedInterface(InPriority),
	GAInsPtr(InGAInsPtr)
{
	bIsOnceTime = true;
}

bool USkill_Passive_XS::FMyStruct::Modify(FGameplayAbilityTargetData_GAReceivedEvent& GameplayAbilityTargetData_GAEvent)
{
	const auto OrginalDamage =
		GameplayAbilityTargetData_GAEvent.Data.TrueDamage +
		GameplayAbilityTargetData_GAEvent.Data.BaseDamage +
		[&] {
		int32 Value = 0;

		for (const auto Iter : GameplayAbilityTargetData_GAEvent.Data.ElementSet)
		{
			Value += Iter.Get<2>();
		}

		return Value;
		}();

	const auto MaxHP =
		GAInsPtr->CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().HP.GetMaxValue();

	const auto CurrentHP =
		GAInsPtr->CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().HP.GetCurrentValue();

	const auto Threshold = MaxHP * GAInsPtr->MinGPPercent;

	if ((CurrentHP - OrginalDamage) < Threshold)
	{
		// 触发阈值 HP
		const auto GreaterThresholdValue = FMath::Max(0, CurrentHP - Threshold);
		
		// 先减去阈值之上的 HP
		const auto OffsetValue = OrginalDamage - GreaterThresholdValue;

		// 可以生成的盾的值
		const auto ShieldValue = MaxHP * GAInsPtr->ShieldValue_HP_Percent;

		// 剩余的盾
		const auto RemaindShield = ShieldValue - OffsetValue;

		int32 ActulyDamage = 0;
		if (RemaindShield > 0)
		{
			ActulyDamage = OrginalDamage - FMath::Max(0, ShieldValue - RemaindShield);
		}
		else
		{
			ActulyDamage = OrginalDamage - ShieldValue;
		}

		// 折算伤害
		const auto Percent = static_cast<float>(ActulyDamage) / OrginalDamage;

		GameplayAbilityTargetData_GAEvent.Data.TrueDamage = GameplayAbilityTargetData_GAEvent.Data.TrueDamage * Percent;
		GameplayAbilityTargetData_GAEvent.Data.BaseDamage = GameplayAbilityTargetData_GAEvent.Data.BaseDamage * Percent;
		for (auto& Iter : GameplayAbilityTargetData_GAEvent.Data.ElementSet)
		{
			Iter.Get<2>() = Iter.Get<2>() * Percent;
		}

		return true;
	}
	else
	{
		return false;
	}
}
