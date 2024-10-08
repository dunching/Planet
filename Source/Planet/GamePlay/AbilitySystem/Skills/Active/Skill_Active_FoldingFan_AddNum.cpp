
#include "Skill_Active_FoldingFan_AddNum.h"

#include "Net/UnrealNetwork.h"

#include "AbilityTask_FlyAway.h"

#include "GameFramework/RootMotionSource.h"

#include "CharacterBase.h"
#include "CharacterStateInfo.h"
#include "StateProcessorComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "AllocationSkills.h"
#include "ProxyProcessComponent.h"
#include "GameplayTagsSubSystem.h"
#include "SceneElement.h"
#include "Skill_WeaponActive_FoldingFan.h"

void USkill_Active_FoldingFan_AddNum::ActivateAbility(
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
		auto TargetSkillSPtr = CharacterPtr->GetProxyProcessComponent()->GetWeaponSkillByType(
			UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Weapon_FoldingFan
		);

		if (TargetSkillSPtr)
		{
			auto RegisterParamPtr =
				Cast<USkill_WeaponActive_FoldingFan>(TargetSkillSPtr->GetGAInst())->RegisterParamSPtr;
			auto GameplayAbilityTargetPtr =
				DeepClone_GameplayAbilityTargetData<FGameplayAbilityTargetData_FoldingFan_RegisterParam>(RegisterParamPtr.Get());

			FGameplayEventData GameplayEventData;
			GameplayEventData.TargetData.Add(GameplayAbilityTargetPtr);

			Cast<UPlanetAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent)->Replicate_UpdateGAParam(
				TargetSkillSPtr->GetGAHandle(),
				GameplayEventData
			);
		}

		CommitAbility(Handle, ActorInfo, ActivationInfo);
		K2_CancelAbility();
	}
#endif
}
