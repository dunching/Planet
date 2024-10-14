
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
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_Tornado.h"
#include "Skill_Active_Tornado.h"

FGameplayAbilityTargetData_StateModify_Stagnation::FGameplayAbilityTargetData_StateModify_Stagnation()
{

}

FGameplayAbilityTargetData_StateModify_Stagnation::FGameplayAbilityTargetData_StateModify_Stagnation(
	float Duration
) :
	Super(UGameplayTagsSubSystem::GetInstance()->State_Buff_Stagnation, Duration)
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
		UGameplayTagsSubSystem::GetInstance()->State_Buff_Stagnation
	);

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
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
		UGameplayTagsSubSystem::GetInstance()->State_Buff_Stagnation
	);

	const float OverrideBlendOutTime = -1.0f;
	Cast<UPlanetAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent)->CurrentMontageStopImp(OverrideBlendOutTime);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_PeriodicStateModify_Stagnation::InitalTags()
{
	Super::InitalTags();

	CancelAbilitiesWithTag.AddTag(UGameplayTagsSubSystem::GetInstance()->Skill_CanBeInterrupted_Stagnation);

	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantPlayerInputMove);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantPathFollowMove);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantRootMotion);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantJump);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantRotation);

	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->State_Buff_Stagnation);

}
