
#include "Skill_Base.h"

#include "SceneElement.h"
#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "PlanetWorldSettings.h"
#include "InteractiveSkillComponent.h"
#include "InteractiveBaseGAComponent.h"

USkill_Base::USkill_Base() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerExecution;
}

void USkill_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// CDO
	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
}

void USkill_Base::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	// Inst
	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
}

void USkill_Base::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool USkill_Base::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

void USkill_Base::CancelAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	bool bReplicateCancelAbility
)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void USkill_Base::OnRemoveAbility(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void USkill_Base::Tick(float DeltaTime)
{
}

bool USkill_Base::GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent) const
{
	return true;
}

const TArray<FAbilityTriggerData>& USkill_Base::GetTriggers() const
{
	return AbilityTriggers;
}

void USkill_Base::ResetPreviousStageActions()
{
	// 清除上一阶段遗留的内容
	for (int32 TaskIdx = ActiveTasks.Num() - 1; TaskIdx >= 0 && ActiveTasks.Num() > 0; --TaskIdx)
	{
		UGameplayTask* Task = ActiveTasks[TaskIdx];
		if (Task)
		{
			Task->TaskOwnerEnded();
		}
	}
	ActiveTasks.Reset();
	ResetListLock();
}

void USkill_Base::SendEvent(const FGameplayEventData& Payload)
{
	if (CharacterPtr)
	{
		auto ASCPtr = CharacterPtr->GetAbilitySystemComponent();
		ASCPtr->TriggerAbilityFromGameplayEvent(
			CharacterPtr->GetInteractiveBaseGAComponent()->SendEventHandle,
			ASCPtr->AbilityActorInfo.Get(),
			FGameplayTag(),
			&Payload,
			*ASCPtr
		);
	}
}
