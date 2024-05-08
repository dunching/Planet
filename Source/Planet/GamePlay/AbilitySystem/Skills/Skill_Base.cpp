
#include "Skill_Base.h"

#include "SceneElement.h"
#include "CharacterBase.h"
#include "EquipmentElementComponent.h"
#include "AbilityTask_TimerHelper.h"

USkill_Base::USkill_Base() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void USkill_Base::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CooldownConsumeTime = CooldownTime - ResetCooldownTime;
}

void USkill_Base::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	OnCurrentStepEnd();
}

void USkill_Base::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		CoolDownGEHanlde = ApplyGameplayEffectToOwner(Handle, ActorInfo, ActivationInfo, CooldownGE, GetAbilityLevel(Handle, ActorInfo));
	}
}

bool USkill_Base::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */)
{
	CooldownConsumeTime = 0.f;

	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

bool USkill_Base::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags /*= nullptr*/, const FGameplayTagContainer* TargetTags /*= nullptr*/, OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */) const
{
	if (CooldownConsumeTime < CooldownTime)
	{
		return false;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_Base::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	RepeatType = ERepeatType::kStop;

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void USkill_Base::Tick(float DeltaTime)
{
	CooldownConsumeTime += DeltaTime;
}

bool USkill_Base::GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent) const
{
	if (CooldownTime < 0.f)
	{
		const auto Remaining = CooldownTime - CooldownConsumeTime;

		if (Remaining <= 0.f)
		{
			RemainingCooldown = 0.f;

			RemainingCooldownPercent = 1.f;

			return true;
		}
		else
		{
			RemainingCooldown = Remaining;

			RemainingCooldownPercent = RemainingCooldown / ResetCooldownTime;

			return false;
		}
	}

	const auto Remaining = CooldownTime - CooldownConsumeTime;

	if (Remaining <= 0.f)
	{
		RemainingCooldown = 0.f;

		RemainingCooldownPercent = 1.f;

		return true;
	}
	else
	{
		RemainingCooldown = Remaining;

		RemainingCooldownPercent = RemainingCooldown / CooldownTime;

		return false;
	}
}

const TArray<FAbilityTriggerData>& USkill_Base::GetTriggers() const
{
	return AbilityTriggers;
}

void USkill_Base::AddCooldownConsumeTime(float NewTime)
{
	CooldownConsumeTime += NewTime;
}

void USkill_Base::SendEvent(const FGameplayEventData& Payload)
{
	auto CharacterPtr = Cast<ACharacterBase>(GetActorInfo().AvatarActor.Get());
	if (CharacterPtr)
	{
		auto ASCPtr = CharacterPtr->GetAbilitySystemComponent();
		ASCPtr->TriggerAbilityFromGameplayEvent(
			CharacterPtr->GetEquipmentItemsComponent()->SendEventHandle,
			ASCPtr->AbilityActorInfo.Get(),
			FGameplayTag(),
			&Payload,
			*ASCPtr
		);
	}
}

void USkill_Base::ExcuteStepsLink()
{

}

void USkill_Base::ExcuteStopStep()
{
}

void USkill_Base::OnCurrentStepEnd()
{
	// 清楚上一阶段遗留的内容
	for (int32 TaskIdx = ActiveTasks.Num() - 1; TaskIdx >= 0 && ActiveTasks.Num() > 0; --TaskIdx)
	{
		UGameplayTask* Task = ActiveTasks[TaskIdx];
		if (Task)
		{
			Task->TaskOwnerEnded();
		}
	}
	ActiveTasks.Reset();
	ResetListLock();

	// 开始下一阶段
	switch (RepeatType)
	{
	case ERepeatType::kCount:
	{
		CurrentRepeatCount++;

		if (CurrentRepeatCount > RepeatCount)
		{
			K2_EndAbility();
		}
		else
		{
			auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
			TaskPtr->SetCount(1);
			TaskPtr->OnFinished.BindLambda([this](auto) {
				WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::OnCurrentStepEnd));
				ExcuteStepsLink();
				RunIfListLock();
				});

			TaskPtr->ReadyForActivation();
		}
	}
	break;
	case ERepeatType::kInfinte:
	{
		auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
		TaskPtr->SetCount(1);
		TaskPtr->OnFinished.BindLambda([this](auto) {
			WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::OnCurrentStepEnd));
			ExcuteStepsLink();
			RunIfListLock();
			});

		TaskPtr->ReadyForActivation();
	}
	break;
	case ERepeatType::kStop:
	default:
	{
		ExcuteStopStep();
	}
	break;
	}
}

