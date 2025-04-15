#include "BasicFutures_HasBeenFlyAway.h"

#include "AbilityTask_ApplyRootMotion_FlyAway.h"
#include "CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UScriptStruct* FGameplayAbilityTargetData_HasBeenFlyAway::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_HasBeenFlyAway::StaticStruct();
}

bool FGameplayAbilityTargetData_HasBeenFlyAway::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << Height;

	return true;
}

FGameplayAbilityTargetData_HasBeenFlyAway* FGameplayAbilityTargetData_HasBeenFlyAway::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_HasBeenFlyAway;

	*ResultPtr = *this;

	return ResultPtr;
}

UBasicFutures_HasBeenFlyAway::UBasicFutures_HasBeenFlyAway():
                                                            Super()
{
	bRetriggerInstancedAbility = true;
}

void UBasicFutures_HasBeenFlyAway::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
		CharacterPtr->LandedDelegate.AddDynamic(this, &ThisClass::OnLanded);
	}
}

void UBasicFutures_HasBeenFlyAway::PreActivate(
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
		ActiveParamSPtr = MakeSPtr_GameplayAbilityTargetData<FActiveParamType>(TriggerEventData->TargetData.Get(0));
		if (ActiveParamSPtr)
		{
		}
		else
		{
			return;
		}
	}
}

void UBasicFutures_HasBeenFlyAway::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		TaskPtr = UAbilityTask_HasBeenFlyAway::NewTask(
			this,
			TEXT(""),
			ActiveParamSPtr->Height
		);
		TaskPtr->Ability = this;

		TaskPtr->ReadyForActivation();
	}
}

void UBasicFutures_HasBeenFlyAway::OnLanded(
	const FHitResult& Hit
)
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		K2_CancelAbility();
	}
#endif
}
