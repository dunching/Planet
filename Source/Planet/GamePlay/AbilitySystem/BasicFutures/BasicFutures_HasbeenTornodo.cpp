#include "BasicFutures_HasbeenTornodo.h"

#include "AbilityTask_ApplyRootMotion_FlyAway.h"
#include "AbilityTask_ARM_RadialForce.h"
#include "AbilityTask_ARM_Tornado.h"
#include "CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UScriptStruct* FGameplayAbilityTargetData_HasbeenTornodo::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_HasbeenTornodo::StaticStruct();
}

bool FGameplayAbilityTargetData_HasbeenTornodo::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << TornadoPtr;

	return true;
}

FGameplayAbilityTargetData_HasbeenTornodo* FGameplayAbilityTargetData_HasbeenTornodo::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_HasbeenTornodo;

	*ResultPtr = *this;

	return ResultPtr;
}

void UBasicFutures_HasbeenTornodo::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	CharacterPtr = Cast<ACharacterBase>(ActorInfo->AvatarActor.Get());
	if (CharacterPtr)
	{
	}
}

void UBasicFutures_HasbeenTornodo::PreActivate(
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

void UBasicFutures_HasbeenTornodo::PerformAction(
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
		auto TaskPtr = UAbilityTask_ARM_Tornado::ApplyRootMotionTornado(
			this,
			TEXT(""),
			ActiveParamSPtr->TornadoPtr
		);
		TaskPtr->Ability = this;
		if (
			(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
		)
		{
			TaskPtr->OnFinished.BindUObject(this, &ThisClass::K2_CancelAbility);
		}

		TaskPtr->ReadyForActivation();
	}
}
