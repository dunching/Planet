
#include "CS_PeriodicStateModify.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>

#include "KismetGravityLibrary.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "InteractiveBaseGAComponent.h"
#include "GameplayTagsSubSystem.h"
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_Tornado.h"
#include "Skill_Active_Tornado.h"

UCS_PeriodicStateModify::UCS_PeriodicStateModify() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UCS_PeriodicStateModify::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_StateModify*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
			AbilityTags.AddTag(GameplayAbilityTargetDataPtr->Tag);
			ActivationOwnedTags.AddTag(GameplayAbilityTargetDataPtr->Tag);
			CancelAbilitiesWithTag.AddTag(GameplayAbilityTargetDataPtr->Tag);
			BlockAbilitiesWithTag.AddTag(GameplayAbilityTargetDataPtr->Tag);
		}
	}

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_PeriodicStateModify::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_PeriodicStateModify::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (EffectItemPtr)
	{
		EffectItemPtr->RemoveFromParent();
		EffectItemPtr = nullptr;
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_PeriodicStateModify::UpdateDuration()
{
	if (TaskPtr)
	{
		TaskPtr->UpdateDuration();
	}
	if (GameplayAbilityTargetDataPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->FlyAway))
	{
		for (auto Iter : ActiveTasks)
		{
			if (Iter->IsA(UAbilityTask_FlyAway::StaticClass()))
			{
				auto RootMotionTaskPtr = Cast<UAbilityTask_FlyAway>(Iter);

				RootMotionTaskPtr->UpdateDuration();
				break;
			}
		}
	}
}

void UCS_PeriodicStateModify::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();
	}
}

void UCS_PeriodicStateModify::ExcuteTasks()
{
	if (CharacterPtr->IsPlayerControlled())
	{
	}
	else
	{

	}

	if (GameplayAbilityTargetDataPtr->Duration < 0.f)
	{
	}
	else
	{
		TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
		TaskPtr->SetDuration(GameplayAbilityTargetDataPtr->Duration);
		TaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::OnInterval);
		TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::OnDuration);
		TaskPtr->OnFinished.BindLambda([this](auto) {
			K2_CancelAbility();
			return true;
			});
		TaskPtr->ReadyForActivation();
	}
}

void UCS_PeriodicStateModify::OnInterval(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval >= Interval)
	{
	}
}

void UCS_PeriodicStateModify::OnDuration(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CharacterPtr->IsPlayerControlled())
	{
		if (CurrentInterval > Interval)
		{
		}
		else
		{
			if (EffectItemPtr)
			{
			}
		}
	}
	else
	{

	}
}

void UCS_PeriodicStateModify::OnTaskComplete()
{
	K2_CancelAbility();
}

FGameplayAbilityTargetData_StateModify::FGameplayAbilityTargetData_StateModify(
	const FGameplayTag& InTag,
	float InDuration
) :
	Duration(InDuration),
	Tag(InTag)
{

}

FGameplayAbilityTargetData_StateModify::FGameplayAbilityTargetData_StateModify()
{

}

FGameplayAbilityTargetData_StateModify* FGameplayAbilityTargetData_StateModify::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_StateModify;

	*ResultPtr = *this;

	return ResultPtr;
}
