
#include "Skill_Passive_XS.h"

#include "AbilitySystemComponent.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
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

	ReigsterEffect();
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

void USkill_Passive_XS::ReigsterEffect()
{
	if (CharacterPtr)
	{
		EventModifyReceivedSPtr = MakeShared<FMyStruct>(300, this);

		CharacterPtr->GetBaseFeatureComponent()->AddReceviedEventModify(EventModifyReceivedSPtr);
	}
}

void USkill_Passive_XS::PerformAction()
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		{
			auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
			TaskPtr->SetDuration(CD, 0.1f);
			TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::CD_DurationDelegate);
			TaskPtr->OnFinished.BindLambda([this](auto) {
				CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CD_CharacterStateInfoSPtr);
				CD_CharacterStateInfoSPtr = nullptr;

				ReigsterEffect();
				return true;
				});
			TaskPtr->ReadyForActivation();
		}
		{
			auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
			TaskPtr->SetDuration(ShieldDuration, 0.1f);
			TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::Duration_DurationDelegate);
			TaskPtr->OnFinished.BindLambda([this](auto) {
				CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(Duration_CharacterStateInfoSPtr);
				Duration_CharacterStateInfoSPtr = nullptr;
				return true;
				});
			TaskPtr->ReadyForActivation();
		}
	}
#endif
}

void USkill_Passive_XS::OnSendAttack(const FGAEventData& GAEventData)
{
}

void USkill_Passive_XS::CD_DurationDelegate(UAbilityTask_TimerHelper* TaskPtr, float CurrentTime, float Duration)
{
	if (CD_CharacterStateInfoSPtr)
	{
		CD_CharacterStateInfoSPtr->TotalTime = Duration - CurrentTime;
		CD_CharacterStateInfoSPtr->DataChanged();
		CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CD_CharacterStateInfoSPtr);
	}
	else
	{
		CD_CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
		CD_CharacterStateInfoSPtr->Tag = SkillUnitPtr->GetUnitType();
		CD_CharacterStateInfoSPtr->Duration = Duration;
		CD_CharacterStateInfoSPtr->DefaultIcon = SkillUnitPtr->GetIcon();
		CD_CharacterStateInfoSPtr->DataChanged();
		CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CD_CharacterStateInfoSPtr);
	}
}

void USkill_Passive_XS::Duration_DurationDelegate(UAbilityTask_TimerHelper* TaskPtr, float CurrentTime, float Duration)
{
	if (Duration_CharacterStateInfoSPtr)
	{
		Duration_CharacterStateInfoSPtr->TotalTime = CurrentTime;
		Duration_CharacterStateInfoSPtr->DataChanged();
		CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(Duration_CharacterStateInfoSPtr);
	}
	else
	{
		Duration_CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
		Duration_CharacterStateInfoSPtr->Tag = SkillUnitPtr->GetUnitType();
		Duration_CharacterStateInfoSPtr->Duration = Duration;
		Duration_CharacterStateInfoSPtr->DefaultIcon = SkillUnitPtr->GetIcon();
		Duration_CharacterStateInfoSPtr->DataChanged();
		CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(Duration_CharacterStateInfoSPtr);
	}
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

		GAInsPtr->PerformAction();

		return true;
	}
	else
	{
		return false;
	}
}
