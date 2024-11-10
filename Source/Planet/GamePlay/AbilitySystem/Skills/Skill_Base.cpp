
#include "Skill_Base.h"

#include "SceneElement.h"
#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "PlanetWorldSettings.h"
#include "ProxyProcessComponent.h"
#include "BaseFeatureComponent.h"
#include "GameOptions.h"
#include "HoldingItemsComponent.h"
#include "PlanetPlayerController.h"

UScriptStruct* FGameplayAbilityTargetData_SkillBase_RegisterParam::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_SkillBase_RegisterParam::StaticStruct();
}

bool FGameplayAbilityTargetData_SkillBase_RegisterParam::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	if (Ar.IsSaving())
	{
		Ar << ProxyID;
	}
	else if (Ar.IsLoading())
	{
		Ar << ProxyID;
	}

	return true;
}

FGameplayAbilityTargetData_SkillBase_RegisterParam* FGameplayAbilityTargetData_SkillBase_RegisterParam::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_SkillBase_RegisterParam;

	*ResultPtr = *this;

	return ResultPtr;
}

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
	if (Spec.GameplayEventData)
	{
		UpdateRegisterParam(*Spec.GameplayEventData);
	}
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

	ResetPreviousStageActions();
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

void USkill_Base::UpdateRegisterParam(const FGameplayEventData& GameplayEventData)
{
	if (GameplayEventData.TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetPtr = MakeSPtr_GameplayAbilityTargetData<FRegisterParamType>(GameplayEventData.TargetData.Get(0));
		if (GameplayAbilityTargetPtr)
		{
			SkillUnitPtr = CharacterPtr->GetHoldingItemsComponent()->FindUnit_Skill(GameplayAbilityTargetPtr->ProxyID);
		}
	}
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

ACharacterBase* USkill_Base::HasFocusActor() const
{
	return CharacterPtr->GetFocusActor();
}

bool USkill_Base::CheckTargetInDistance(int32 InDistance)const
{
	if (CharacterPtr->IsPlayerControlled())
	{
		auto PCPtr = CharacterPtr->GetController<APlanetPlayerController>();
		auto TargetCharacterPtr = Cast<ACharacterBase>(PCPtr->GetFocusActor());
		if (TargetCharacterPtr)
		{
			return FVector::Distance(TargetCharacterPtr->GetActorLocation(), CharacterPtr->GetActorLocation()) < InDistance;
		}
	}
	else
	{
		auto ACPtr = CharacterPtr->GetController<AAIController>();
		auto TargetCharacterPtr = Cast<ACharacterBase>(ACPtr->GetFocusActor());
		if (TargetCharacterPtr)
		{
			return FVector::Distance(TargetCharacterPtr->GetActorLocation(), CharacterPtr->GetActorLocation()) < InDistance;
		}
	}

	return false;
}

ACharacterBase* USkill_Base::GetTargetInDistance(int32 Distance) const
{
	return nullptr;
}
