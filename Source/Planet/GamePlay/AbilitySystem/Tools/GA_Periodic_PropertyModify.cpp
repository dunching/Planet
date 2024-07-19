
#include "GA_Periodic_PropertyModify.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"

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

UGA_Periodic_PropertyModify::UGA_Periodic_PropertyModify():
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Periodic_PropertyModify::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_Periodic_PropertyModify*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
		}
	}

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UGA_Periodic_PropertyModify::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UGA_Periodic_PropertyModify::EndAbility(
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

void UGA_Periodic_PropertyModify::UpdateDuration()
{
	if (TaskPtr)
	{
		TaskPtr->UpdateDuration();
	}
}

void UGA_Periodic_PropertyModify::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();
	}
}

void UGA_Periodic_PropertyModify::ExcuteTasks()
{
	if (CharacterPtr->IsPlayerControlled())
	{
		auto EffectPtr = UUIManagerSubSystem::GetInstance()->ViewEffectsList(true);
		if (EffectPtr)
		{
			EffectItemPtr = EffectPtr->AddEffectItem();
			EffectItemPtr->SetTexutre(GameplayAbilityTargetDataPtr->DefaultIcon);
		}
	}
	else
	{

	}

	TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
	TaskPtr->SetDuration(GameplayAbilityTargetDataPtr->Duration, GameplayAbilityTargetDataPtr->PerformActionInterval);
	TaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::OnInterval);
	TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::OnDuration);
	TaskPtr->OnFinished.BindLambda([this](auto) {
		K2_CancelAbility();
		});
	TaskPtr->ReadyForActivation();
}

void UGA_Periodic_PropertyModify::OnInterval(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval >= Interval)
	{
	}
}

void UGA_Periodic_PropertyModify::OnDuration(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
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
				EffectItemPtr->SetPercent(true, (Interval - CurrentInterval) / Interval);
			}
		}
	}
	else
	{

	}
}

FGameplayAbilityTargetData_Periodic_PropertyModify::FGameplayAbilityTargetData_Periodic_PropertyModify(UConsumableUnit* RightVal)
{
	Duration = RightVal->Duration;
	PerformActionInterval = RightVal->PerformActionInterval;
	ModifyPropertyMap = RightVal->ModifyPropertyMap;
	DefaultIcon = RightVal->GetIcon();
}

FGameplayAbilityTargetData_Periodic_PropertyModify::FGameplayAbilityTargetData_Periodic_PropertyModify()
{

}
