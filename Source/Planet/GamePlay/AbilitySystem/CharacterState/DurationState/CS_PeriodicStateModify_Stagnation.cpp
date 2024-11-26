
#include "CS_PeriodicStateModify_Stagnation.h"

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
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"

FGameplayAbilityTargetData_StateModify_Stagnation::FGameplayAbilityTargetData_StateModify_Stagnation()
{

}

FGameplayAbilityTargetData_StateModify_Stagnation::FGameplayAbilityTargetData_StateModify_Stagnation(
	float Duration
) :
	Super(UGameplayTagsSubSystem::State_Buff_Stagnation, Duration)
{
}

void UCS_PeriodicStateModify_Stagnation::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void UCS_PeriodicStateModify_Stagnation::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Cast<UPlanetAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent)->AddLooseGameplayTag_2_Client(
		UGameplayTagsSubSystem::State_Buff_Stagnation
	);

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_PeriodicStateModify_Stagnation::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_PeriodicStateModify_Stagnation::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Cast<UPlanetAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent)->RemoveLooseGameplayTag_2_Client(
		UGameplayTagsSubSystem::State_Buff_Stagnation
	);

	const float OverrideBlendOutTime = -1.0f;
	Cast<UPlanetAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent)->CurrentMontageStopImp(OverrideBlendOutTime);

	CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_PeriodicStateModify_Stagnation::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	AbilityTags.AddTag(UGameplayTagsSubSystem::State_Buff_Stagnation);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::State_Buff_Stagnation);

	CancelAbilitiesWithTag.AddTag(UGameplayTagsSubSystem::Skill_CanBeInterrupted_Stagnation);

	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_CantPlayerInputMove);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_CantPathFollowMove);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_CantRootMotion);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_CantJump);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::MovementStateAble_CantRotation);
}

void UCS_PeriodicStateModify_Stagnation::PerformAction()
{
	Super::PerformAction();

	CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
	CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
	CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
	CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
	CharacterStateInfoSPtr->DataChanged();
	CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);
}

void UCS_PeriodicStateModify_Stagnation::OnDuration(
	UAbilityTask_TimerHelper* TaskPtr,
	float CurrentTime,
	float DurationTime
)
{
	CharacterStateInfoSPtr->TotalTime = CurrentTime;
	CharacterStateInfoSPtr->DataChanged();

	CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);
}
