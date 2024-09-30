
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
			AbilityTags.AddTag(GameplayAbilityTargetDataPtr->Tag);
			ActivationOwnedTags.AddTag(GameplayAbilityTargetDataPtr->Tag);
			CancelAbilitiesWithTag.AddTag(GameplayAbilityTargetDataPtr->Tag);
			BlockAbilitiesWithTag.AddTag(GameplayAbilityTargetDataPtr->Tag);
		}
	}

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_RootMotion::UpdateDuration()
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
