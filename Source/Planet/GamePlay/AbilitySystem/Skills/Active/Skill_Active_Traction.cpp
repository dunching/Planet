#include "Skill_Active_Traction.h"

#include <Engine/OverlapResult.h>

#include "AbilityTask_ARM_RadialForce.h"
#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"
#include "CameraTrailHelper.h"
#include "AbilityTask_ControlCameraBySpline.h"
#include "CharacterAttibutes.h"
#include "CharacterAttributesComponent.h"
#include "KismetGravityLibrary.h"
#include "SPlineActor.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"
#include "AbilityTask_PlayMontage.h"
#include "Components/CapsuleComponent.h"

void USkill_Active_Traction::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		if (TractionPoint)
		{
			TractionPoint->Destroy();
		}

		CommitAbility(Handle, ActorInfo, ActivationInfo);
	}
#endif

	TractionPoint = nullptr;

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_Traction::PerformAction(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() > ROLE_SimulatedProxy)
	{
		TractionPoint = GetWorld()->SpawnActor<ATractionPoint>(CharacterPtr->GetActorLocation(), FRotator::ZeroRotator);

		float OutRadius = 0.f;
		float OutHalfHeight = 0.f;
		CharacterPtr->GetCapsuleComponent()->GetScaledCapsuleSize(OutRadius, OutHalfHeight);
		// 
		auto RoomtMotionTaskptr = UAbilityTask_ARM_RadialForce::MyApplyRootMotionRadialForce(
			this,
			TEXT(""),
			TractionPoint,
			Strength,
			Duration,
			Radius,
			OutRadius,
			false
		);

		RoomtMotionTaskptr->ReadyForActivation();
	}

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode() == NM_DedicatedServer)
	{
		auto DelayTaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
		DelayTaskPtr->SetDuration(Duration);
		DelayTaskPtr->OnFinished.BindLambda([this](auto)
		{
			K2_CancelAbility();
			
			return true;
		});
		DelayTaskPtr->ReadyForActivation();
	}
#endif

	PlayMontage();
}

void USkill_Active_Traction::PlayMontage()
{
	if (
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_Authority) ||
		(GetAbilitySystemComponentFromActorInfo()->GetOwnerRole() == ROLE_AutonomousProxy)
	)
	{
		const float InPlayRate = 1.f;

		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontagePtr,
			InPlayRate,
			StartSection
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetCharacterAbilitySystemComponent());

		TaskPtr->ReadyForActivation();
	}
}
