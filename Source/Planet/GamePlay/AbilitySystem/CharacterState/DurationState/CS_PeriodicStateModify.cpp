
#include "CS_PeriodicStateModify.h"

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
		auto GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_StateModify*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
			SetCache(TSharedPtr<FGameplayAbilityTargetData_StateModify>(GameplayAbilityTargetDataPtr->Clone()));

			AbilityTags.AddTag(GameplayAbilityTargetDataSPtr->Tag);
			ActivationOwnedTags.AddTag(GameplayAbilityTargetDataSPtr->Tag);
			CancelAbilitiesWithTag.AddTag(GameplayAbilityTargetDataSPtr->Tag);
			BlockAbilitiesWithTag.AddTag(GameplayAbilityTargetDataSPtr->Tag);
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

	ExcuteTasks();
}

void UCS_PeriodicStateModify::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_PeriodicStateModify::UpdateDuration()
{
}

void UCS_PeriodicStateModify::SetCache(const TSharedPtr<FGameplayAbilityTargetData_StateModify>& InGameplayAbilityTargetDataSPtr)
{
	GameplayAbilityTargetDataSPtr = InGameplayAbilityTargetDataSPtr;
}

void UCS_PeriodicStateModify::PerformAction()
{
	if (CharacterPtr)
	{
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

	if (GameplayAbilityTargetDataSPtr->Duration < 0.f)
	{
	}
	else
	{
		TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
		TaskPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration);
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
	if (CurrentInterval > Interval)
	{
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
	Super(InTag),
	Duration(InDuration)
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
