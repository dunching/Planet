
#include "CS_PeriodicStateModify_Purify.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>

#include "KismetGravityLibrary.h"
#include "NavigationSystem.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "AbilityTask_ARM_ConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "StateProcessorComponent.h"
#include "CharacterStateInfo.h"
#include "PlanetPlayerController.h"
#include "CharacterAttibutes.h"

FGameplayAbilityTargetData_StateModify_Purify::FGameplayAbilityTargetData_StateModify_Purify() :
	Super(UGameplayTagsLibrary::State_Buff_Purify, .1f)
{
}

void UCS_PeriodicStateModify_Purify::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void UCS_PeriodicStateModify_Purify::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_PeriodicStateModify_Purify::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_PeriodicStateModify_Purify::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_PeriodicStateModify_Purify::UpdateDuration()
{
	Super::UpdateDuration();

	if (TimerTaskPtr)
	{
		TimerTaskPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration);
		TimerTaskPtr->UpdateDuration();
	}
}

void UCS_PeriodicStateModify_Purify::PerformAction()
{
}

void UCS_PeriodicStateModify_Purify::OnTaskTick(UAbilityTask_TimerHelper*, float DeltaTime)
{
}

void UCS_PeriodicStateModify_Purify::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	AbilityTags.AddTag(UGameplayTagsLibrary::State_Buff_Purify);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_Buff_Purify);

	TArray<FGameplayTag>Ary{
		UGameplayTagsLibrary::State_Debuff_Stun,
		UGameplayTagsLibrary::State_Debuff_Charm,
		UGameplayTagsLibrary::State_Debuff_Fear,
		UGameplayTagsLibrary::State_Debuff_Silent,
		UGameplayTagsLibrary::State_Debuff_Slow,
	};

	for (const auto& Iter : Ary)
	{
		CancelAbilitiesWithTag.AddTag(Iter);
	}

	for (const auto& Iter : Ary)
	{
		BlockAbilitiesWithTag.AddTag(Iter);
	}
}
