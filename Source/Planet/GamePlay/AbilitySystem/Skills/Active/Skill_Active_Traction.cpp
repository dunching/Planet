
#include "Skill_Active_Traction.h"

#include <Engine/OverlapResult.h>

#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAbilitySystemComponent.h"
#include "CameraTrailHelper.h"
#include "AbilityTask_ControlCameraBySpline.h"
#include "CharacterAttibutes.h"
#include "CharacterAttributesComponent.h"
#include "KismetGravityLibrary.h"
#include "KismetCollisionHelper.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"
#include "AbilityTask_PlayMontage.h"

void USkill_Active_Traction::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
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

#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);

		{
			auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
			TaskPtr->SetDuration(Duration, 0.1f);
			TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::DurationDelegate);
			TaskPtr->OnFinished.BindLambda([this](auto)
				{
					K2_CancelAbility();
					return true;
				});
			TaskPtr->ReadyForActivation();
		}

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.Owner = CharacterPtr;
	}
#endif

	PlayMontage();
}

void USkill_Active_Traction::IntervalDelegate(
	UAbilityTask_TimerHelper*,
	float CurrentIntervalTime, 
	float IntervalTime,
	bool bIsEnd
)
{
}

void USkill_Active_Traction::DurationDelegate(UAbilityTask_TimerHelper*, float CurrentIntervalTime, float IntervalTime)
{
#if UE_EDITOR || UE_SERVER
	if (GetAbilitySystemComponentFromActorInfo()->GetNetMode()  == NM_DedicatedServer)
	{
		if (CharacterStateInfoSPtr)
		{
			CharacterStateInfoSPtr->TotalTime = CurrentIntervalTime;
			CharacterStateInfoSPtr->DataChanged();
		}
	}
#endif
}

void USkill_Active_Traction::PlayMontage()
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
