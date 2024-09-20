
#include "CS_RootMotion_FlyAway.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>

#include "KismetGravityLibrary.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "UnitProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "BaseFeatureGAComponent.h"
#include "GameplayTagsSubSystem.h"
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_Tornado.h"
#include "Skill_Active_Tornado.h"
#include "CharacterStateInfo.h"

void UCS_RootMotion_FlyAway::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_RootMotion_FlyAway*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
			SetCache(TSharedPtr<FGameplayAbilityTargetData_RootMotion_FlyAway>(GameplayAbilityTargetDataPtr->Clone()));
		}
	}

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_RootMotion_FlyAway::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_RootMotion_FlyAway::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_RootMotion_FlyAway::UpdateDuration()
{
	if (AbilityTask_TimerHelperPtr)
	{
		AbilityTask_TimerHelperPtr->UpdateDuration();
	}
	if (RootMotionTaskPtr)
	{
		RootMotionTaskPtr->UpdateDuration();
	}
}

void UCS_RootMotion_FlyAway::SetCache(const TSharedPtr<FGameplayAbilityTargetData_RootMotion_FlyAway>& InGameplayAbilityTargetDataSPtr)
{
	GameplayAbilityTargetDataSPtr = InGameplayAbilityTargetDataSPtr;
}

void UCS_RootMotion_FlyAway::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();

		RootMotionTaskPtr = UAbilityTask_FlyAway::NewTask(
			this,
			TEXT(""),
			GameplayAbilityTargetDataSPtr->Duration,
			GameplayAbilityTargetDataSPtr->Height
		);

		RootMotionTaskPtr->Ability = this;
		RootMotionTaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

		RootMotionTaskPtr->OnFinish.BindUObject(this, &ThisClass::DecrementToZeroListLock);

		RootMotionTaskPtr->ReadyForActivation();
	}
}

void UCS_RootMotion_FlyAway::ExcuteTasks()
{
	if (CharacterPtr->IsPlayerControlled())
	{
	}
	else
	{

	}

	if (GameplayAbilityTargetDataSPtr->Duration < 0.f)
	{
	}
	else
	{
		AbilityTask_TimerHelperPtr = UAbilityTask_TimerHelper::DelayTask(this);
		AbilityTask_TimerHelperPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration);
		AbilityTask_TimerHelperPtr->IntervalDelegate.BindUObject(this, &ThisClass::OnInterval);
		AbilityTask_TimerHelperPtr->DurationDelegate.BindUObject(this, &ThisClass::OnDuration);
		AbilityTask_TimerHelperPtr->OnFinished.BindLambda([this](auto) {
			K2_CancelAbility();
			return true;
			});
		AbilityTask_TimerHelperPtr->ReadyForActivation();
	}
}

void UCS_RootMotion_FlyAway::OnTaskComplete()
{
	K2_CancelAbility();
}

void UCS_RootMotion_FlyAway::OnInterval(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval >= Interval)
	{
	}
}

void UCS_RootMotion_FlyAway::OnDuration(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval > Interval)
	{
	}
	else
	{
	}
}

FGameplayAbilityTargetData_RootMotion_FlyAway::FGameplayAbilityTargetData_RootMotion_FlyAway():
	Super(UGameplayTagsSubSystem::GetInstance()->FlyAway)
{

}

FGameplayAbilityTargetData_RootMotion_FlyAway* FGameplayAbilityTargetData_RootMotion_FlyAway::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_RootMotion_FlyAway;

	*ResultPtr = *this;

	return ResultPtr;
}
