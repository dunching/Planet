
#include "CS_RootMotion.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>

#include "KismetGravityLibrary.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "InteractiveBaseGAComponent.h"
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
