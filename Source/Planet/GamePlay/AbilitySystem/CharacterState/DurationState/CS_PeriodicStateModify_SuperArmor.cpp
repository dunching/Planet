
#include "CS_PeriodicStateModify_SuperArmor.h"

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
#include "BaseFeatureComponent.h"
#include "GameplayTagsSubSystem.h"
#include "AbilityTask_ARM_ConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "StateProcessorComponent.h"
#include "CharacterStateInfo.h"
#include "PlanetPlayerController.h"
#include "CharacterAttibutes.h"

FGameplayAbilityTargetData_StateModify_SuperArmor::FGameplayAbilityTargetData_StateModify_SuperArmor(
	float Duration
) :
	Super(UGameplayTagsSubSystem::State_Buff_SuperArmor, Duration)
{
}

FGameplayAbilityTargetData_StateModify_SuperArmor::FGameplayAbilityTargetData_StateModify_SuperArmor()
{

}

void UCS_PeriodicStateModify_SuperArmor::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void UCS_PeriodicStateModify_SuperArmor::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_PeriodicStateModify_SuperArmor::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_PeriodicStateModify_SuperArmor::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_PeriodicStateModify_SuperArmor::UpdateDuration()
{
	Super::UpdateDuration();

	if (TimerTaskPtr)
	{
		TimerTaskPtr->SetDuration(GameplayAbilityTargetDataSPtr->Duration);
		TimerTaskPtr->UpdateDuration();
	}

	if (CharacterStateInfoSPtr)
	{
		CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
		CharacterStateInfoSPtr->TotalTime = 0.f;
	}
}

void UCS_PeriodicStateModify_SuperArmor::PerformAction()
{
	// 
	CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
	CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
	CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
	CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
	CharacterStateInfoSPtr->DataChanged();
	CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);
}

void UCS_PeriodicStateModify_SuperArmor::OnTaskTick(UAbilityTask_TimerHelper*, float DeltaTime)
{
	CharacterStateInfoSPtr->TotalTime += DeltaTime;

	CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);
}

void UCS_PeriodicStateModify_SuperArmor::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	AbilityTags.AddTag(UGameplayTagsSubSystem::State_Buff_SuperArmor);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::State_Buff_SuperArmor);

	TArray<FGameplayTag>Ary{
		UGameplayTagsSubSystem::State_Debuff_Stun,
		UGameplayTagsSubSystem::State_Debuff_Charm,
		UGameplayTagsSubSystem::State_Debuff_Fear,
		UGameplayTagsSubSystem::State_Debuff_Silent,
		UGameplayTagsSubSystem::State_Debuff_Slow,
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
