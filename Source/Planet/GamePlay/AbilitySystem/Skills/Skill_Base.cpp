
#include "Skill_Base.h"

#include "SceneElement.h"
#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "PlanetWorldSettings.h"
#include "UnitProxyProcessComponent.h"
#include "BaseFeatureGAComponent.h"
#include "GameOptions.h"

USkill_Base::USkill_Base() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void USkill_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// CDO
	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());

	// 远程不能复制这个参数？
// 	if (Spec.GameplayEventData.IsValid() && Spec.GameplayEventData->TargetData.IsValid(0))
// 	{
// 		auto GameplayAbilityTargetPtr = dynamic_cast<const FGameplayAbilityTargetData_Skill*>(Spec.GameplayEventData->TargetData.Get(0));
// 		if (GameplayAbilityTargetPtr)
// 		{
// 			SkillUnitPtr = GameplayAbilityTargetPtr->SkillUnitPtr;
// 		}
// 	}
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

bool USkill_Base::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/,
	const FGameplayTagContainer* TargetTags /*= nullptr*/,
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
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

UScriptStruct* FGameplayAbilityTargetData_Skill::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_Skill::StaticStruct();
}

bool FGameplayAbilityTargetData_Skill::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		SkillUnitPtr->NetSerialize(Ar, Map, bOutSuccess);
	}
	else if (Ar.IsLoading())
	{
		auto TempPtr = MakeShared<FSkillProxy>();
		TempPtr->NetSerialize(Ar, Map, bOutSuccess);

		SkillUnitPtr = DynamicCastSharedPtr<FSkillProxy>(TempPtr->GetThisSPtr());
	}

	return true;
}

FGameplayAbilityTargetData_Skill* FGameplayAbilityTargetData_Skill::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_Skill;

	*ResultPtr = *this;

	return ResultPtr;
}
