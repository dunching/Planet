
#include "GA_Tool_Periodic.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>

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

UGA_Tool_Periodic::UGA_Tool_Periodic()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Tool_Periodic::PreActivate(
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
		GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_Tool_Periodic*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
		}
	}
}

void UGA_Tool_Periodic::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UGA_Tool_Periodic::EndAbility(
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

void UGA_Tool_Periodic::UpdateDuration()
{
	if (TaskPtr)
	{
		TaskPtr->UpdateDuration();
	}
}

void UGA_Tool_Periodic::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();
	}
}

void UGA_Tool_Periodic::ExcuteTasks()
{
	auto EffectPtr = UUIManagerSubSystem::GetInstance()->ViewEffectsList(true);
	if (EffectPtr)
	{
		EffectItemPtr = EffectPtr->AddEffectItem();
		FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
		AsyncLoadTextureHandle = StreamableManager.RequestAsyncLoad(GameplayAbilityTargetDataPtr->DefaultIcon.ToSoftObjectPath(), [this]()
			{
				EffectItemPtr->SetTexutre(GameplayAbilityTargetDataPtr->DefaultIcon);
			});
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

void UGA_Tool_Periodic::OnInterval(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval >= Interval)
	{
	}
}

void UGA_Tool_Periodic::OnDuration(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
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

const FGameplayAbilityTargetData_Tool_Periodic& FGameplayAbilityTargetData_Tool_Periodic::operator=(UConsumableUnit* RightVal)
{
	Duration = RightVal->Duration;
	PerformActionInterval = RightVal->PerformActionInterval;
	ModifyPropertyMap = RightVal->ModifyPropertyMap;
	DefaultIcon = RightVal->GetIcon();

	return *this;
}
