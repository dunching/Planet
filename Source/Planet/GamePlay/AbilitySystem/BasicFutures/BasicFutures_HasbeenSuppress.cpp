#include "BasicFutures_HasbeenSuppress.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "AbilityTask_ApplyRootMotion_FlyAway.h"
#include "AbilityTask_ARM_RadialForce.h"
#include "AbilityTask_ARM_Tornado.h"
#include "AbilityTask_PlayMontage.h"
#include "AssetRefMap.h"
#include "CharacterAbilitySystemComponent.h"
#include "CharacterBase.h"
#include "GameplayTagsManager.h"
#include "KismetGravityLibrary.h"
#include "StateProcessorComponent.h"
#include "Kismet/KismetMathLibrary.h"

UScriptStruct* FGameplayAbilityTargetData_ActiveParam_HasbeenSuppress::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_ActiveParam_HasbeenSuppress::StaticStruct();
}

bool FGameplayAbilityTargetData_ActiveParam_HasbeenSuppress::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << MontageRef;
	Ar << InstigatorPtr;

	return true;
}

FGameplayAbilityTargetData_ActiveParam_HasbeenSuppress* FGameplayAbilityTargetData_ActiveParam_HasbeenSuppress::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_ActiveParam_HasbeenSuppress;

	*ResultPtr = *this;

	return ResultPtr;
}

void UBasicFutures_HasbeenSuppress::OnAvatarSet(
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

void UBasicFutures_HasbeenSuppress::PreActivate(
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

void UBasicFutures_HasbeenSuppress::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
	)
{
	CharacterPtr->GetStateProcessorComponent()->RemoveGetOrientFunc(
	                                                                GetOrientPrority
	                                                               );

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		GetAbilitySystemComponentFromActorInfo()->RemoveActiveGameplayEffect(SuppressGEHandle);
	}
#endif

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UBasicFutures_HasbeenSuppress::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
	)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CharacterPtr->GetStateProcessorComponent()->AddGetOrientFunc(
	                                                             GetOrientPrority,
	                                                             std::bind(
	                                                                       &ThisClass::GetOrient,
	                                                                       this,
	                                                                       std::placeholders::_1,
	                                                                       std::placeholders::_2
	                                                                      )
	                                                            );

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		auto SpecHandle =
			MakeOutgoingGameplayEffectSpec(UAssetRefMap::GetInstance()->SuppressClass, GetAbilityLevel());

		SuppressGEHandle = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
	}
#endif

	const float InPlayRate = 1.f;
	auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
	                                                                          this,
	                                                                          TEXT(""),
	                                                                          ActiveParamSPtr->MontageRef.
	                                                                          LoadSynchronous(),
	                                                                          InPlayRate
	                                                                         );

	TaskPtr->Ability = this;
	TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority)
	)
	{
		TaskPtr->OnCompleted.BindUObject(this, &ThisClass::K2_CancelAbility);
		TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);
	}
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		TaskPtr->ReadyForActivation();
	}
}

bool UBasicFutures_HasbeenSuppress::GetOrient(
	FRotator& DesiredRotation,
	bool& bIsImmediatelyRot
	)
{
	if (ActiveParamSPtr && ActiveParamSPtr->InstigatorPtr.IsValid())
	{
		if (CharacterPtr)
		{
			const auto CurrentLocation = CharacterPtr->GetActorLocation();
			// Normalized
			const auto Z = -UKismetGravityLibrary::GetGravity();
			DesiredRotation = UKismetMathLibrary::MakeRotFromZX(
			                                                    Z,
			                                                    ActiveParamSPtr->InstigatorPtr->GetActorLocation() -
			                                                    CurrentLocation
			                                                   );
			bIsImmediatelyRot = true;
			return true;
		}
	}

	return false;
}
