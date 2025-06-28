
#include "PlanetGameplayAbility.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "LogWriter.h"

#include "PlanetAbilitySystemComponent.h"
#include "TemplateHelper.h"
#include "Kismet/KismetStringLibrary.h"

UScriptStruct* FGameplayAbilityTargetData_RegisterParam::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_RegisterParam::StaticStruct();
}

bool FGameplayAbilityTargetData_RegisterParam::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
)
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

bool FGameplayAbilityTargetData_ActiveParam::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
)
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

#if WITH_EDITOR
void UPlanetGameplayAbility::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// 确认和我们运行的GA标签一致
	// InitalDefaultTags();
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
	
	PerformActionWrap(Handle, ActorInfo, ActivationInfo, TriggerEventData);
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

void UPlanetGameplayAbility::OnGameplayTaskInitialized(
	UGameplayTask& Task
)
{
	Super::OnGameplayTaskInitialized(Task);
}

void UPlanetGameplayAbility::OnGameplayTaskActivated(
	UGameplayTask& Task
)
{
	Super::OnGameplayTaskActivated(Task);
}

void UPlanetGameplayAbility::OnGameplayTaskDeactivated(
	UGameplayTask& Task
)
{
	Super::OnGameplayTaskDeactivated(Task);
}

#endif

void UPlanetGameplayAbility::UpdateRegisterParam(
	const FGameplayEventData& GameplayEventData
	)
{
	if (GameplayEventData.TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetPtr = MakeSPtr_GameplayAbilityTargetData<FRegisterParamType>(
			 GameplayEventData.TargetData.Get(0)
			);
	}
}

void UPlanetGameplayAbility::PerformActionWrap(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	PRINTFUNCSTR(TEXT(""));
	PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UPlanetGameplayAbility::SetContinuePerform(
	bool bIsContinue_
)
{
	PRINTFUNCSTR(UKismetStringLibrary::Conv_BoolToString(bIsContinue_));
	bIsContinueAction = bIsContinue_;
	if (bIsContinueAction)
	{
		
	}
	else
	{
		OnStopContinuePerform();
	}
}

void UPlanetGameplayAbility::OnStopContinuePerform()
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

void UPlanetGameplayAbility::ResetListLock() const
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

bool UPlanetGameplayAbility::CanOncemorePerformAction() const
{
	return false;
}

bool UPlanetGameplayAbility::GetIsContinue() const
{
	PRINTFUNCSTR(UKismetStringLibrary::Conv_BoolToString(bIsContinue_));
	return bIsContinueAction;
}

void UPlanetGameplayAbility::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	bIsContinueAction = true;
}

// void UPlanetGameplayAbility::InitalDefaultTags()
// {
//
// }

void UPlanetGameplayAbility::CancelAbility_Server_Implementation()
{
	K2_CancelAbility();
}
