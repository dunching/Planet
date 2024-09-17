
#include "Skill_Active_BYWD.h"

#include "Net/UnrealNetwork.h"

#include "AbilityTask_FlyAway.h"

#include "GameFramework/RootMotionSource.h"
#include "CharacterBase.h"

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

	if (
		(CharacterPtr->GetLocalRole() == ROLE_Authority) ||
		(CharacterPtr->GetLocalRole() == ROLE_AutonomousProxy)
		)
	{
		auto TaskPtr = UAbilityTask_FlyAway::NewTask(
			this,
			TEXT(""),
			ERootMotionAccumulateMode::Additive,
			3.f,
			250.f
		);

		TaskPtr->OnFinish.BindLambda([this]
			{
				K2_CancelAbility();
			});

		TaskPtr->ReadyForActivation();
	}
}
