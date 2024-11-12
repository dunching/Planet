
#include "CS_PeriodicStateModify.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>

#include "KismetGravityLibrary.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "BaseFeatureComponent.h"
#include "GameplayTagsSubSystem.h"
#include "AbilityTask_ARM_ConstantForce.h"
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

void UCS_PeriodicStateModify::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (Spec.GameplayEventData && Spec.GameplayEventData->TargetData.IsValid(0))
	{
		SetCache(MakeSPtr_GameplayAbilityTargetData<FStateParam>(Spec.GameplayEventData->TargetData.Get(0)));
	}
}

void UCS_PeriodicStateModify::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
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
		TimerTaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
		TimerTaskPtr->SetInfinite();
		TimerTaskPtr->TickDelegate.BindUObject(this, &ThisClass::OnTaskTick);
		TimerTaskPtr->OnFinished.BindLambda([this](auto) {
			K2_CancelAbility();
			return true;
			});
		TimerTaskPtr->ReadyForActivation();
	}
	else
	{
		TimerTaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
		TimerTaskPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration);
		TimerTaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::OnInterval);
		TimerTaskPtr->DurationDelegate.BindUObject(this, &ThisClass::OnDuration);
		TimerTaskPtr->TickDelegate.BindUObject(this, &ThisClass::OnTaskTick);
		TimerTaskPtr->OnFinished.BindLambda([this](auto) {
			K2_CancelAbility();
			return true;
			});
		TimerTaskPtr->ReadyForActivation();
	}
}

void UCS_PeriodicStateModify::OnInterval(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval >= Interval)
	{
	}
}

void UCS_PeriodicStateModify::OnDuration(
	UAbilityTask_TimerHelper* ,
	float CurrentTime,
	float DurationTime
)
{
	if (CurrentTime > DurationTime)
	{
	}
	else
	{
	}
}

void UCS_PeriodicStateModify::OnTaskTick(
	UAbilityTask_TimerHelper*, 
	float DeltaTime
)
{

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

UScriptStruct* FGameplayAbilityTargetData_StateModify::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_StateModify::StaticStruct();
}

bool FGameplayAbilityTargetData_StateModify::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << Duration;
	Ar << bIsCancelState;
	Ar << TriggerCharacterPtr;
	Ar << TargetCharacterPtr;

	return true;
}

FGameplayAbilityTargetData_StateModify* FGameplayAbilityTargetData_StateModify::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_StateModify;

	*ResultPtr = *this;

	return ResultPtr;
}

TSharedPtr<FGameplayAbilityTargetData_StateModify> FGameplayAbilityTargetData_StateModify::Clone_SmartPtr() const
{
	return TSharedPtr<FGameplayAbilityTargetData_StateModify>(Clone());
}
