
#include "CS_RootMotion_MoveAlongSpline.h"

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

void UCS_RootMotion_MoveAlongSpline::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_RootMotion_MoveAlongSpline*>(TriggerEventData->TargetData.Get(0));
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

void UCS_RootMotion_MoveAlongSpline::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_RootMotion_MoveAlongSpline::EndAbility(
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

void UCS_RootMotion_MoveAlongSpline::UpdateDuration()
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

void UCS_RootMotion_MoveAlongSpline::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();

		auto RootMotionTaskPtr = UAbilityTask_ApplyRootMotionBySPline::NewTask(
			this,
			TEXT(""),
			GameplayAbilityTargetDataPtr->Duration,
			GameplayAbilityTargetDataPtr->SPlineActorPtr.Get(),
			GameplayAbilityTargetDataPtr->TargetCharacterPtr.Get()
		);

		RootMotionTaskPtr->Ability = this;
		RootMotionTaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

		RootMotionTaskPtr->OnFinish.BindUObject(this, &ThisClass::DecrementToZeroListLock);

		RootMotionTaskPtr->ReadyForActivation();
	}
}

void UCS_RootMotion_MoveAlongSpline::ExcuteTasks()
{
	if (CharacterPtr->IsPlayerControlled())
	{
		auto EffectPtr = UUIManagerSubSystem::GetInstance()->ViewEffectsList(true);
		if (EffectPtr)
		{
			EffectItemPtr = EffectPtr->AddEffectItem();
		}
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
			});
		TaskPtr->ReadyForActivation();
	}
}

void UCS_RootMotion_MoveAlongSpline::OnTaskComplete()
{
	K2_CancelAbility();
}

void UCS_RootMotion_MoveAlongSpline::OnInterval(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval >= Interval)
	{
	}
}

void UCS_RootMotion_MoveAlongSpline::OnDuration(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
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

FGameplayAbilityTargetData_RootMotion_MoveAlongSpline::FGameplayAbilityTargetData_RootMotion_MoveAlongSpline() :
	Super(UGameplayTagsSubSystem::GetInstance()->MoveAlongSpline)
{

}

FGameplayAbilityTargetData_RootMotion_MoveAlongSpline* FGameplayAbilityTargetData_RootMotion_MoveAlongSpline::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_RootMotion_MoveAlongSpline;

	*ResultPtr = *this;

	return ResultPtr;
}
