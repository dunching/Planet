
#include "Skill_Active_CantBeSelected.h"

#include <Engine/OverlapResult.h>

#include "CharacterBase.h"
#include "AbilityTask_TimerHelper.h"
#include "CS_RootMotion.h"
#include "GameplayTagsLibrary.h"
#include "BaseFeatureComponent.h"
#include "CameraTrailHelper.h"
#include "AbilityTask_ControlCameraBySpline.h"
#include "CharacterAttibutes.h"
#include "CharacterAttributesComponent.h"
#include "KismetGravityLibrary.h"
#include "CS_PeriodicStateModify_Stagnation.h"
#include "CS_PeriodicStateModify_CantBeSelected.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"

void USkill_Active_CantBeSelected::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);
}

void USkill_Active_CantBeSelected::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		CommitAbility(Handle, ActorInfo, ActivationInfo);
		ExcuteTasks();

		CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
		CharacterStateInfoSPtr->Tag = SkillUnitPtr->GetUnitType();
		CharacterStateInfoSPtr->Duration = Duration;
		CharacterStateInfoSPtr->DefaultIcon = SkillUnitPtr->GetIcon();
		CharacterStateInfoSPtr->DataChanged();

		CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

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
	}
#endif
}

void USkill_Active_CantBeSelected::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_CantBeSelected::ExcuteTasks()
{
	if (CharacterPtr)
	{
		{
			auto GameplayAbilityTargetData_RootMotionPtr = new FGameplayAbilityTargetData_StateModify_CantBeSelected(Duration);

			GameplayAbilityTargetData_RootMotionPtr->TriggerCharacterPtr = CharacterPtr;
			GameplayAbilityTargetData_RootMotionPtr->TargetCharacterPtr = CharacterPtr;

			auto ICPtr = CharacterPtr->GetBaseFeatureComponent();

			ICPtr->SendEventImp(GameplayAbilityTargetData_RootMotionPtr);
		}
		{
			auto GameplayAbilityTargetData_RootMotionPtr = new FGameplayAbilityTargetData_StateModify_Stagnation(Duration);

			GameplayAbilityTargetData_RootMotionPtr->TriggerCharacterPtr = CharacterPtr;
			GameplayAbilityTargetData_RootMotionPtr->TargetCharacterPtr = CharacterPtr;

			auto ICPtr = CharacterPtr->GetBaseFeatureComponent();

			ICPtr->SendEventImp(GameplayAbilityTargetData_RootMotionPtr);
		}
	}
}

void USkill_Active_CantBeSelected::DurationDelegate(UAbilityTask_TimerHelper*, float CurrentIntervalTime, float IntervalTime)
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (CharacterStateInfoSPtr)
		{
			CharacterStateInfoSPtr->TotalTime = CurrentIntervalTime;
			CharacterStateInfoSPtr->DataChanged();
			CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(CharacterStateInfoSPtr);
		}
	}
#endif
}
