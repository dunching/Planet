
#include "Skill_Active_Base.h"

#include "SceneElement.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "PlanetWorldSettings.h"
#include "PlanetPlayerController.h"
#include "GameOptions.h"

USkill_Active_Base::USkill_Active_Base():
	Super()
{
	CooldownTime = 1.f;
}

void USkill_Active_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

	CooldownConsumeTime = CooldownTime - UGameOptions::GetInstance()->ResetCooldownTime;
}

void USkill_Active_Base::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetPtr = dynamic_cast<const FGameplayAbilityTargetData_ActiveSkill*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetPtr)
		{
			CanbeActivedInfoSPtr = GameplayAbilityTargetPtr->CanbeActivedInfoSPtr;
		}
	}
}

bool USkill_Active_Base::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	CooldownConsumeTime = 0.f;

	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

bool USkill_Active_Base::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	if (CooldownConsumeTime < CooldownTime)
	{
		return false;
	}

	if (IsActive())
	{
		return true;
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_Active_Base::CancelAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility
)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void USkill_Active_Base::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	WaitInputTaskPtr = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_Base::AddCooldownConsumeTime(float NewTime)
{
	CooldownConsumeTime += NewTime;
}

void USkill_Active_Base::GetInputRemainPercent(bool& bIsAcceptInput, float& Percent) const
{
	if (WaitInputTaskPtr)
	{
		bIsAcceptInput = true;
		Percent = WaitInputPercent;
	}
	else
	{
		bIsAcceptInput = false;
		Percent = 0.f;
	}
}

void USkill_Active_Base::WaitInput()
{
	if (bIsPreviouInput)
	{
		PerformAction();
		bIsPreviouInput = false;
	}
	else
	{
		WaitInputPercent = 1.f;

		WaitInputTaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
		WaitInputTaskPtr->SetDuration(CurrentWaitInputTime, 0.1f);
		WaitInputTaskPtr->DurationDelegate.BindUObject(this , &ThisClass::WaitInputTick);
		WaitInputTaskPtr->OnFinished.BindLambda([this](auto) {
			K2_CancelAbility();
			});
		WaitInputTaskPtr->ReadyForActivation();
	}
}

void USkill_Active_Base::ContinueActive()
{
	if (WaitInputTaskPtr)
	{
		WaitInputTaskPtr->ExternalCancel();
		WaitInputTaskPtr = nullptr;

		PerformAction();
	}
	else
	{
		bIsPreviouInput = true;
	}
}

ACharacterBase * USkill_Active_Base::HasFocusActor() const
{
	if (CharacterPtr->IsPlayerControlled())
	{
		auto PCPtr = CharacterPtr->GetController<APlanetPlayerController>();
		auto TargetCharacterPtr = Cast<ACharacterBase>(PCPtr->GetFocusActor());
		if (TargetCharacterPtr)
		{
			return TargetCharacterPtr;
		}
	}
	else
	{
		auto ACPtr = CharacterPtr->GetController<AAIController>();
		auto TargetCharacterPtr = Cast<ACharacterBase>(ACPtr->GetFocusActor());
		if (TargetCharacterPtr)
		{
			return TargetCharacterPtr;
		}
	}
	return nullptr;
}

bool USkill_Active_Base::CheckTargetInDistance(int32 InDistance)const
{
	if (CharacterPtr->IsPlayerControlled())
	{
		auto PCPtr = CharacterPtr->GetController<APlanetPlayerController>();
		auto TargetCharacterPtr = Cast<ACharacterBase>(PCPtr->GetFocusActor());
		if (TargetCharacterPtr)
		{
			return FVector::Distance(TargetCharacterPtr->GetActorLocation(), CharacterPtr->GetActorLocation()) < InDistance;
		}
	}
	else
	{
		auto ACPtr = CharacterPtr->GetController<AAIController>();
		auto TargetCharacterPtr = Cast<ACharacterBase>(ACPtr->GetFocusActor());
		if (TargetCharacterPtr)
		{
			return FVector::Distance(TargetCharacterPtr->GetActorLocation(), CharacterPtr->GetActorLocation()) < InDistance;
		}
	}

	return false;
}

ACharacterBase* USkill_Active_Base::GetTargetInDistance(int32 Distance) const
{
	return nullptr;
}

void USkill_Active_Base::WaitInputTick(UAbilityTask_TimerHelper*, float Interval, float Duration)
{
	if (Duration > 0.f)
	{
		WaitInputPercent =  FMath::Clamp(1.f - (Interval / Duration), 0.f, 1.f);
	}
	else
	{
		check(0);
		WaitInputPercent = 1.f;
	}
}

void USkill_Active_Base::Tick(float DeltaTime)
{
	CooldownConsumeTime += DeltaTime;
}

bool USkill_Active_Base::GetRemainingCooldown(
	float& RemainingCooldown, float& RemainingCooldownPercent
) const
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

			RemainingCooldownPercent = RemainingCooldown / UGameOptions::GetInstance()->ResetCooldownTime;

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

void USkill_Active_Base::PerformAction()
{

}

FGameplayAbilityTargetData_ActiveSkill* FGameplayAbilityTargetData_ActiveSkill::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_ActiveSkill;

	*ResultPtr = *this;

	return ResultPtr;
}
