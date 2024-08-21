
#include "CS_RootMotion_KnockDown.h"

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
#include "AbilityTask_PlayMontage.h"

void UCS_RootMotion_KnockDown::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	if (TriggerEventData && TriggerEventData->TargetData.IsValid(0))
	{
		GameplayAbilityTargetDataPtr = dynamic_cast<const FGameplayAbilityTargetData_RootMotion_KnockDown*>(TriggerEventData->TargetData.Get(0));
		if (GameplayAbilityTargetDataPtr)
		{
		}
	}

	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_RootMotion_KnockDown::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_RootMotion_KnockDown::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_RootMotion_KnockDown::UpdateDuration()
{
	PerformAction();
}

void UCS_RootMotion_KnockDown::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();
	}
}

void UCS_RootMotion_KnockDown::ExcuteTasks()
{
	if (CharacterPtr->GetCharacterMovement()->IsFlying() || CharacterPtr->GetCharacterMovement()->IsFalling())
	{
		TaskPtr = UAbilityTask_MyApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
			this,
			TEXT(""),
			UKismetGravityLibrary::GetGravity(FVector::ZeroVector),
			GameplayAbilityTargetDataPtr->KnockDownSpeed,
			-1.f,
			false,
			true,
			nullptr,
			ERootMotionFinishVelocityMode::SetVelocity,
			FVector::ZeroVector,
			0.f,
			false
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		TaskPtr->ReadyForActivation();

		CharacterPtr->LandedDelegate.AddDynamic(this, &ThisClass::OnLanding);
	}
	else
	{
		PlayMontage(HumanMontagePtr, 1.f);
	}
}

void UCS_RootMotion_KnockDown::OnTaskComplete()
{
	K2_CancelAbility();
}

void UCS_RootMotion_KnockDown::OnLanding(const FHitResult& Hit)
{
	PlayMontage(HumanMontagePtr, 1.f);
}

void UCS_RootMotion_KnockDown::PlayMontage(UAnimMontage* CurMontagePtr, float Rate)
{
	auto AbilityTask_PlayMontage_PickAxePtr = UAbilityTask_PlayMontage::CreatePlayMontageAndWaitProxy(
		this,
		TEXT(""),
		CurMontagePtr,
		CharacterPtr->GetMesh()->GetAnimInstance(),
		Rate
	);

	AbilityTask_PlayMontage_PickAxePtr->Ability = this;
	AbilityTask_PlayMontage_PickAxePtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
	AbilityTask_PlayMontage_PickAxePtr->OnCompleted.BindUObject(this, &ThisClass::OnMontageComplete);
	AbilityTask_PlayMontage_PickAxePtr->OnInterrupted.BindUObject(this, &ThisClass::OnMontageComplete);

	AbilityTask_PlayMontage_PickAxePtr->ReadyForActivation();
}

void UCS_RootMotion_KnockDown::OnMontageComplete()
{
	K2_CancelAbility();
}

FGameplayAbilityTargetData_RootMotion_KnockDown::FGameplayAbilityTargetData_RootMotion_KnockDown() :
	Super(UGameplayTagsSubSystem::GetInstance()->FlyAway)
{

}

FGameplayAbilityTargetData_RootMotion_KnockDown* FGameplayAbilityTargetData_RootMotion_KnockDown::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_RootMotion_KnockDown;

	*ResultPtr = *this;

	return ResultPtr;
}
