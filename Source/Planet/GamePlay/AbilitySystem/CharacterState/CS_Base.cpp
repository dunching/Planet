
#include "CS_Base.h"

#include "CharacterBase.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"

FGameplayAbilityTargetData_CS_Base::FGameplayAbilityTargetData_CS_Base(
	const FGameplayTag& InTag
) :
	Tag(InTag)
{

}

FGameplayAbilityTargetData_CS_Base::FGameplayAbilityTargetData_CS_Base()
{

}

FGameplayAbilityTargetData_CS_Base* FGameplayAbilityTargetData_CS_Base::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_CS_Base;

	*ResultPtr = *this;

	return ResultPtr;
}

UCS_Base::UCS_Base() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
}

void UCS_Base::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	// CDO
	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
}

void UCS_Base::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetDataBasePtr = dynamic_cast<const FGameplayAbilityTargetData_CS_Base*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataBasePtr)
		{
			GameplayAbilityTargetDataBaseSPtr = TSharedPtr<FGameplayAbilityTargetData_CS_Base>(GameplayAbilityTargetDataBasePtr->Clone());

			GameplayAbilityTargetDataBaseSPtr->CharacterStateChanged.ExcuteCallback(ECharacterStateType::kActive, this);
		}
	}
}

void UCS_Base::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	if (GameplayAbilityTargetDataBaseSPtr)
	{
		GameplayAbilityTargetDataBaseSPtr->CharacterStateChanged.ExcuteCallback(ECharacterStateType::kEnd, this);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_Base::UpdateDuration()
{

}
