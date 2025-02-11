
#include "CS_PeriodicStateModify_Stun.h"

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
#include "CharacterAbilitySystemComponent.h"
#include "GameplayTagsLibrary.h"
#include "AbilityTask_ARM_ConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_Tornado.h"
#include "Skill_Active_Tornado.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"

FGameplayAbilityTargetData_StateModify_Stun::FGameplayAbilityTargetData_StateModify_Stun(
	float Duration
):
	Super(UGameplayTagsLibrary::State_Debuff_Stun, Duration)
{
}

FGameplayAbilityTargetData_StateModify_Stun::FGameplayAbilityTargetData_StateModify_Stun() 
{

}

void UCS_PeriodicStateModify_Stun::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void UCS_PeriodicStateModify_Stun::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_PeriodicStateModify_Stun::EndAbility(
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

void UCS_PeriodicStateModify_Stun::UpdateDuration()
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

void UCS_PeriodicStateModify_Stun::PerformAction()
{
	Super::PerformAction();

	CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
	CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
	CharacterStateInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
	CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
	CharacterStateInfoSPtr->DataChanged();
	CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);
}

void UCS_PeriodicStateModify_Stun::OnTaskTick(UAbilityTask_TimerHelper*, float DeltaTime)
{
	CharacterStateInfoSPtr->TotalTime += DeltaTime;

	CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);
}

void UCS_PeriodicStateModify_Stun::InitalDefaultTags()
{
	Super::InitalDefaultTags();

	// AbilityTags.AddTag(UGameplayTagsLibrary::State_Buff_Stagnation);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::State_Buff_Stagnation);

	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantPlayerInputMove);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantPathFollowMove);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantJump);
	ActivationOwnedTags.AddTag(UGameplayTagsLibrary::MovementStateAble_CantRotation);
}
