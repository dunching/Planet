
#include "CS_PeriodicStateModify_CantBeSelected.h"

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
#include "StateProcessorComponent.h"

FGameplayAbilityTargetData_StateModify_CantBeSelected::FGameplayAbilityTargetData_StateModify_CantBeSelected()
{

}

FGameplayAbilityTargetData_StateModify_CantBeSelected::FGameplayAbilityTargetData_StateModify_CantBeSelected(
	float Duration
) :
	Super(UGameplayTagsSubSystem::State_Buff_CantBeSlected, Duration)
{
}

void UCS_PeriodicStateModify_CantBeSelected::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void UCS_PeriodicStateModify_CantBeSelected::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_PeriodicStateModify_CantBeSelected::EndAbility(
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

void UCS_PeriodicStateModify_CantBeSelected::PerformAction()
{
	Super::PerformAction();

	CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
	CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
	CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
	CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
	CharacterStateInfoSPtr->DataChanged();
	CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);
}

void UCS_PeriodicStateModify_CantBeSelected::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	AbilityTags.AddTag(UGameplayTagsSubSystem::State_Buff_CantBeSlected);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::State_Buff_CantBeSlected);
}

void UCS_PeriodicStateModify_CantBeSelected::OnDuration(
	UAbilityTask_TimerHelper* TaskPtr,
	float CurrentTime,
	float DurationTime
)
{
	CharacterStateInfoSPtr->TotalTime = CurrentTime;
	CharacterStateInfoSPtr->DataChanged();

	CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);
}
