
#include "Skill_Active_BYWD.h"

#include "Net/UnrealNetwork.h"

#include "AbilityTask_FlyAway.h"

#include "GameFramework/RootMotionSource.h"

#include "CharacterBase.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"
#include "AbilityTask_TimerHelper.h"

void USkill_Active_BYWD::ActivateAbility(
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
	}
#endif
	
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		StateDisplayInfoSPtr = MakeShared<FCharacterStateInfo>();
		StateDisplayInfoSPtr->Tag = SkillUnitPtr->GetUnitType();
		StateDisplayInfoSPtr->Duration = Duration;
		StateDisplayInfoSPtr->DefaultIcon = SkillUnitPtr->GetIcon();
		StateDisplayInfoSPtr->DataChanged();

		CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(StateDisplayInfoSPtr);

		{
			auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
			TaskPtr->SetDuration(Duration, 0.1f);
			TaskPtr->TickDelegate.BindUObject(this, &ThisClass::TimerTick);
			TaskPtr->ReadyForActivation();
		}
	}
#endif

	if (
		(CharacterPtr->GetLocalRole() == ROLE_Authority) ||
		(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
	{
		{
			auto TaskPtr = UAbilityTask_FlyAway::NewTask(
				this,
				TEXT(""),
				ERootMotionAccumulateMode::Additive,
				Duration,
				Height
			);

#if UE_EDITOR || UE_SERVER
			if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
			{
				TaskPtr->OnFinish.BindLambda([this]
					{
						CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(StateDisplayInfoSPtr);

						K2_CancelAbility();
					});
			}
#endif

			TaskPtr->ReadyForActivation();
		}
	}
}

void USkill_Active_BYWD::TimerTick(UAbilityTask_TimerHelper*, float Interval)
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (StateDisplayInfoSPtr)
		{
			StateDisplayInfoSPtr->TotalTime += Interval;
			StateDisplayInfoSPtr->DataChanged();
			CharacterPtr->GetStateProcessorComponent()->ChangeStateDisplay(StateDisplayInfoSPtr);
		}
	}
#endif
}
