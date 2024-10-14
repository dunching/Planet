// Copyright 2020 Dan Kestranek.


#include "PlanetGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"

#include "PlanetAbilitySystemComponent.h"

UScriptStruct* FGameplayAbilityTargetData_RegisterParam::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_RegisterParam::StaticStruct();
}

bool FGameplayAbilityTargetData_RegisterParam::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	return true;
}

FGameplayAbilityTargetData_RegisterParam* FGameplayAbilityTargetData_RegisterParam::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_RegisterParam;

	*ResultPtr = *this;

	return ResultPtr;
}

bool FGameplayAbilityTargetData_ActiveParam::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << ID;

	return true;
}

FGameplayAbilityTargetData_ActiveParam* FGameplayAbilityTargetData_ActiveParam::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_ActiveParam;

	*ResultPtr = *this;

	return ResultPtr;
}

UPlanetGameplayAbility::UPlanetGameplayAbility() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UPlanetGameplayAbility::SetContinuePerform(bool bIsContinue)
{
	if (CurrentActorInfo && CurrentActorInfo->AbilitySystemComponent.IsValid())
	{
		Cast<UPlanetAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent)->ReplicateContinues(
			CurrentSpecHandle, 
			CurrentActivationInfo, 
			bIsContinue
		);
	}

	SetContinuePerformImp(bIsContinue);
}

#if WITH_EDITOR
void UPlanetGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// 确认和我们运行的GA标签一致
	InitalTags();
}

void UPlanetGameplayAbility::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, 
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UPlanetGameplayAbility::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, 
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool UPlanetGameplayAbility::CommitAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

bool UPlanetGameplayAbility::CanActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayTagContainer* SourceTags /*= nullptr*/, 
	const FGameplayTagContainer* TargetTags /*= nullptr*/, 
	OUT FGameplayTagContainer* OptionalRelevantTags /*= nullptr */
) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UPlanetGameplayAbility::CancelAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility
)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void UPlanetGameplayAbility::EndAbility(
	const FGameplayAbilitySpecHandle Handle, 
	const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, 
	bool bReplicateEndAbility, 
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UPlanetGameplayAbility::OnGameplayTaskInitialized(UGameplayTask& Task)
{
	Super::OnGameplayTaskInitialized(Task);
}

void UPlanetGameplayAbility::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);
}

void UPlanetGameplayAbility::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskDeactivated(Task);
}
#endif

void UPlanetGameplayAbility::SetContinuePerformImp(bool bIsContinue)
{

}

void UPlanetGameplayAbility::RunIfListLock() const
{
	if (ScopeLockCount <= 0)
	{
		ScopeLockCount = 0;

		auto Temp = std::move(WaitingToExecute);

		WaitingToExecute.Empty();

		for (int32 Idx = 0; Idx < Temp.Num(); ++Idx)
		{
			Temp[Idx].ExecuteIfBound();
		}
	}
}

void UPlanetGameplayAbility::ResetListLock()const
{
	ScopeLockCount = 0;
	WaitingToExecute.Empty();
}

void UPlanetGameplayAbility::DecrementToZeroListLock() const
{
	ScopeLockCount = 0;
	RunIfListLock();
}

void UPlanetGameplayAbility::DecrementListLockOverride() const
{
	ScopeLockCount--;
	RunIfListLock();
}

void UPlanetGameplayAbility::InitalTags()
{

}

