
#include "CS_RootMotion.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>

#include "KismetGravityLibrary.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "BaseFeatureComponent.h"
#include "GameplayTagsSubSystem.h"
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_Tornado.h"
#include "Skill_Active_Tornado.h"

UCS_RootMotion::UCS_RootMotion() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateYes;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UCS_RootMotion::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		auto GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_RootMotion*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
		}
	}

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_RootMotion::UpdateRootMotion_Implementation(
	const FGameplayEventData& GameplayEventData
)
{
	auto tDataSPtr =
		MakeSPtr_GameplayAbilityTargetData<FGameplayAbilityTargetData_RootMotion>(GameplayEventData.TargetData.Get(0));
	if (CharacterPtr->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		TArray<UGameplayAbility*> Instances = GetCurrentAbilitySpec()->GetAbilityInstances();

		for (auto Instance : Instances)
		{
			Cast<UCS_RootMotion>(Instance)->UpdateRootMotionImp(tDataSPtr);
		}
	}
	else
	{
		UpdateRootMotionImp(tDataSPtr);
	}
}

void UCS_RootMotion::InitalDefaultTags()
{
	Super::InitalDefaultTags();
}

void UCS_RootMotion::UpdateRootMotionImp(const TSharedPtr<FGameplayAbilityTargetData_RootMotion>& DataSPtr)
{

}

void UCS_RootMotion::PerformAction()
{
	if (CharacterPtr)
	{
	}
}

FGameplayAbilityTargetData_RootMotion::FGameplayAbilityTargetData_RootMotion(
	const FGameplayTag& InTag
	):
	Super(InTag)
{

}

FGameplayAbilityTargetData_RootMotion::FGameplayAbilityTargetData_RootMotion()
{

}

UScriptStruct* FGameplayAbilityTargetData_RootMotion::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_RootMotion::StaticStruct();
}

bool FGameplayAbilityTargetData_RootMotion::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	if (Ar.IsSaving())
	{
		Ar << TriggerCharacterPtr;
		Ar << TargetCharacterPtr;

	}
	else if (Ar.IsLoading())
	{
		Ar << TriggerCharacterPtr;
		Ar << TargetCharacterPtr;

	}

	return true;
}

FGameplayAbilityTargetData_RootMotion* FGameplayAbilityTargetData_RootMotion::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_RootMotion;

	*ResultPtr = *this;

	return ResultPtr;
}

TSharedPtr<FGameplayAbilityTargetData_RootMotion> FGameplayAbilityTargetData_RootMotion::Clone_SmartPtr() const
{
	return TSharedPtr<FGameplayAbilityTargetData_RootMotion>(Clone());
}
