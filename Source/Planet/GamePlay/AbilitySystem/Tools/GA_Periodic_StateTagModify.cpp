
#include "GA_Periodic_StateTagModify.h"

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

UGA_Periodic_StateTagModify::UGA_Periodic_StateTagModify() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_Periodic_StateTagModify::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_Periodic_StateTagModify*>(TriggerEventData->TargetData.Get(0));
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

void UGA_Periodic_StateTagModify::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UGA_Periodic_StateTagModify::EndAbility(
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

void UGA_Periodic_StateTagModify::UpdateDuration()
{
	if (TaskPtr)
	{
		TaskPtr->UpdateDuration();
	}
}

void UGA_Periodic_StateTagModify::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();

		if (GameplayAbilityTargetDataPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->FlyAway))
		{
			auto RootMotionTaskPtr = UAbilityTask_FlyAway::NewTask(
				this,
				TEXT(""),
				GameplayAbilityTargetDataPtr->Height
			);

			RootMotionTaskPtr->Ability = this;
			RootMotionTaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());

			// 如果遇到障碍 提前结束
			RootMotionTaskPtr->OnFinish.BindUObject(this, &ThisClass::DecrementToZeroListLock);

			RootMotionTaskPtr->ReadyForActivation();

		}
		else if (GameplayAbilityTargetDataPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->Silent))
		{

		}
		else if (GameplayAbilityTargetDataPtr->Tag.MatchesTagExact(UGameplayTagsSubSystem::GetInstance()->Stun))
		{

		}
	}
}

void UGA_Periodic_StateTagModify::ExcuteTasks()
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
	TaskPtr->SetDuration(GameplayAbilityTargetDataPtr->Duration);
	TaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::OnInterval);
	TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::OnDuration);
	TaskPtr->OnFinished.BindLambda([this](auto) {
		K2_CancelAbility();
		});
	TaskPtr->ReadyForActivation();
}

void UGA_Periodic_StateTagModify::OnInterval(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval >= Interval)
	{
	}
}

void UGA_Periodic_StateTagModify::OnDuration(UAbilityTask_TimerHelper* InTaskPtr, float CurrentInterval, float Interval)
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

FGameplayAbilityTargetData_Periodic_StateTagModify::FGameplayAbilityTargetData_Periodic_StateTagModify(
	const FGameplayTag& InTag,
	float InDuration
	):
	Duration(InDuration),
	Tag(InTag)
{

}

FGameplayAbilityTargetData_Periodic_StateTagModify::FGameplayAbilityTargetData_Periodic_StateTagModify()
{

}
