#include "BasicFutures_HasbeenTraction.h"

#include "AbilityTask_ApplyRootMotion_FlyAway.h"
#include "AbilityTask_ARM_RadialForce.h"
#include "CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UScriptStruct* FGameplayAbilityTargetData_HasbeenTraction::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_HasbeenTraction::StaticStruct();
}

bool FGameplayAbilityTargetData_HasbeenTraction::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << TractionPoint;

	return true;
}

FGameplayAbilityTargetData_HasbeenTraction* FGameplayAbilityTargetData_HasbeenTraction::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_HasbeenTraction;

	*ResultPtr = *this;

	return ResultPtr;
}

void UBasicFutures_HasbeenTraction::OnAvatarSet(
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

void UBasicFutures_HasbeenTraction::PreActivate(
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

void UBasicFutures_HasbeenTraction::PerformAction(
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
		auto TaskPtr = UAbilityTask_ARM_RadialForce::ApplyRootMotionRadialForce(
			this,
			TEXT(""),
			ActiveParamSPtr->TractionPoint
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
